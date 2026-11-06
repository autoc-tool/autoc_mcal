/*
 * Host regression tests for the PC MCAL EcuBus transport.
 *
 * Covers the two failures seen against ecb_cli on 127.0.0.1:17320:
 *   - Can.Init must send controllerId equal to the EcuBus handle (not the
 *     AUTOSAR controller index).
 *   - libc read/write interrupted by a NuttX tick (EINTR, errno 4) must be
 *     retried, or Can_RpcCall drops the socket and Can_Init stays UNINIT.
 *
 * The EcuBus server itself is not required. A local peer speaks the same
 * one-line JSON-RPC framing.
 */

#define _POSIX_C_SOURCE 200809L

#include "Can_rpc.h"
#include "can_rpc_host.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

void can_rpc_host_test_reset(void);
unsigned can_rpc_host_test_eintr_retries(void);

static int g_failures;

#define EXPECT(cond)                                                                                       \
	do {                                                                                               \
		if (!(cond)) {                                                                             \
			fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                   \
			g_failures++;                                                                      \
		}                                                                                          \
	} while (0)

static void sleep_ms(unsigned ms)
{
	struct timespec ts;

	ts.tv_sec = (time_t)(ms / 1000u);
	ts.tv_nsec = (long)(ms % 1000u) * 1000000L;
	while (nanosleep(&ts, &ts) != 0) {
		if (errno != EINTR) {
			break;
		}
	}
}

static long elapsed_ms(const struct timespec *start)
{
	struct timespec now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	return (long)(now.tv_sec - start->tv_sec) * 1000L + (long)(now.tv_nsec - start->tv_nsec) / 1000000L;
}

static void on_usr1_nop(int signo)
{
	(void)signo;
}

static void arm_sigusr1(void)
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	/* No SA_RESTART: a tick must surface as EINTR, matching NuttX sim. */
	sa.sa_handler = on_usr1_nop;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	EXPECT(sigaction(SIGUSR1, &sa, NULL) == 0);
}

static int wait_for_eintr(pthread_t target, unsigned base, int budget_ms)
{
	struct timespec start;

	clock_gettime(CLOCK_MONOTONIC, &start);
	while (can_rpc_host_test_eintr_retries() == base) {
		if (pthread_kill(target, SIGUSR1) != 0) {
			return -1;
		}
		if (elapsed_ms(&start) > budget_ms) {
			return -1;
		}
		sleep_ms(1);
	}
	return 0;
}

static int write_all(int fd, const void *buf, size_t len)
{
	const char *p = (const char *)buf;
	size_t left = len;

	while (left > 0u) {
		ssize_t n = write(fd, p, left);
		if ((n < 0) && (errno == EINTR)) {
			continue;
		}
		if (n <= 0) {
			return -1;
		}
		p += n;
		left -= (size_t)n;
	}
	return 0;
}

static int read_line(int fd, char *buf, size_t cap)
{
	size_t off = 0u;

	if (cap < 2u) {
		return -1;
	}
	while (off + 1u < cap) {
		ssize_t n = read(fd, buf + off, cap - 1u - off);
		if ((n < 0) && (errno == EINTR)) {
			continue;
		}
		if (n <= 0) {
			break;
		}
		off += (size_t)n;
		buf[off] = '\0';
		if (strchr(buf, '\n') != NULL) {
			return 0;
		}
	}
	buf[off] = '\0';
	return -1;
}

static int listen_loopback(uint16 *port)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int fd;
	int one = 1;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		return -1;
	}
	(void)setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(0);
	if ((bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) || (listen(fd, 1) != 0) ||
	    (getsockname(fd, (struct sockaddr *)&addr, &len) != 0)) {
		close(fd);
		return -1;
	}
	*port = ntohs(addr.sin_port);
	return fd;
}

struct rpc_peer {
	int listen_fd;
	pthread_t client;
	int interrupt_read;
	int drop;
	int chunk;
	int saw_eintr;
	char req[8192];
};

static void *rpc_peer_thread(void *arg)
{
	struct rpc_peer *peer = (struct rpc_peer *)arg;
	int fd = accept(peer->listen_fd, NULL, NULL);
	const char *resp = "{\"jsonrpc\":\"2.0\",\"result\":{\"result\":\"E_OK\",\"resultCode\":0,\"initialized\":true},\"id\":1}\n";
	unsigned base;

	if (fd < 0) {
		return NULL;
	}
	if (read_line(fd, peer->req, sizeof(peer->req)) != 0) {
		close(fd);
		return NULL;
	}
	if (peer->drop) {
		close(fd);
		return NULL;
	}
	if (peer->interrupt_read) {
		base = can_rpc_host_test_eintr_retries();
		peer->saw_eintr = (wait_for_eintr(peer->client, base, 2000) == 0);
	}
	if (peer->chunk) {
		size_t i;
		for (i = 0u; resp[i] != '\0'; i++) {
			if (write_all(fd, &resp[i], 1u) != 0) {
				close(fd);
				return NULL;
			}
		}
	} else if (write_all(fd, resp, strlen(resp)) != 0) {
		close(fd);
		return NULL;
	}
	/* Keep the socket until the client finishes reading. */
	sleep_ms(50);
	close(fd);
	return NULL;
}

static const Can_ControllerConfigType g_controllers[] = {
	{
		.controller_id = 0u,
		.activation = TRUE,
		.name = "CanController_0",
		.vendor = "simulate",
		.handle = 1u,
		.canfd = FALSE,
		.bitrate_bps = 500000u,
		.fd_bitrate_bps = 0u,
		.brs = FALSE,
		.hardware_name = "CanController_0",
		.baud_id = 0u,
	},
	{
		.controller_id = 2u,
		.activation = TRUE,
		.name = "CanFd",
		.vendor = "simulate",
		.handle = 5u,
		.canfd = TRUE,
		.bitrate_bps = 500000u,
		.fd_bitrate_bps = 2000000u,
		.brs = TRUE,
		.hardware_name = "CanFd",
		.baud_id = 1u,
	},
};

static const Can_HardwareObjectConfigType g_hohs[] = {
	{
		.hoh_id = 0u,
		.controller_id = 0u,
		.object_type = CAN_OBJECT_TRANSMIT,
		.handle_type = CAN_HANDLE_BASIC,
		.id_type = CAN_ID_STANDARD,
		.padding = 0u,
		.has_filter = FALSE,
		.filter_code = 0u,
		.filter_mask = 0u,
	},
	{
		.hoh_id = 1u,
		.controller_id = 0u,
		.object_type = CAN_OBJECT_RECEIVE,
		.handle_type = CAN_HANDLE_BASIC,
		.id_type = CAN_ID_STANDARD,
		.padding = 0u,
		.has_filter = TRUE,
		.filter_code = 0u,
		.filter_mask = 0u,
	},
	{
		.hoh_id = 2u,
		.controller_id = 2u,
		.object_type = CAN_OBJECT_TRANSMIT,
		.handle_type = CAN_HANDLE_FULL,
		.id_type = CAN_ID_EXTENDED,
		.padding = 0u,
		.has_filter = FALSE,
		.filter_code = 0u,
		.filter_mask = 0u,
	},
};

static int run_init_against_peer(struct rpc_peer *peer, uint16 port)
{
	Can_RpcConfigType rpc;
	Can_ConfigType cfg;
	pthread_t thread;
	Std_ReturnType rc;

	memset(&rpc, 0, sizeof(rpc));
	rpc.host = "127.0.0.1";
	rpc.port = port;
	rpc.socket_path = "";
	rpc.auto_reconnect = FALSE;
	rpc.reconnect_interval_ms = 0u;

	memset(&cfg, 0, sizeof(cfg));
	cfg.rpc = &rpc;
	cfg.controllers = g_controllers;
	cfg.controller_count = (uint8)(sizeof(g_controllers) / sizeof(g_controllers[0]));
	cfg.hohs = g_hohs;
	cfg.hoh_count = (uint16)(sizeof(g_hohs) / sizeof(g_hohs[0]));

	peer->client = pthread_self();
	if (pthread_create(&thread, NULL, rpc_peer_thread, peer) != 0) {
		return -1;
	}
	rc = Can_RpcConnect(&rpc);
	if (rc == E_OK) {
		rc = Can_RpcInitFromConfig(&cfg);
	}
	Can_RpcDisconnect();
	Can_RpcClearAttempt();
	(void)pthread_join(thread, NULL);
	return (rc == E_OK) ? 0 : -1;
}

static void test_init_sends_handle_as_controller_id(void)
{
	struct rpc_peer peer;
	uint16 port = 0;
	int lfd = listen_loopback(&port);
	const char *classic =
		"{\"vendor\":\"simulate\",\"handle\":1,\"controllerId\":1,\"name\":\"CanController_0\","
		"\"canfd\":false,\"bitrate\":{\"freq\":500000}}";
	const char *fd =
		"{\"vendor\":\"simulate\",\"handle\":5,\"controllerId\":5,\"name\":\"CanFd\",\"canfd\":true,"
		"\"bitrate\":{\"freq\":500000},\"bitratefd\":{\"freq\":2000000}}";
	const char *tx =
		"{\"hohId\":0,\"controllerId\":1,\"objectType\":\"TRANSMIT\",\"handleType\":\"BASIC\",\"idType\":\"STANDARD\"}";
	const char *rx =
		"{\"hohId\":1,\"controllerId\":1,\"objectType\":\"RECEIVE\",\"handleType\":\"BASIC\","
		"\"idType\":\"STANDARD\",\"canId\":0,\"idMask\":0}";
	const char *tx_fd =
		"{\"hohId\":2,\"controllerId\":5,\"objectType\":\"TRANSMIT\",\"handleType\":\"FULL\",\"idType\":\"EXTENDED\"}";

	fprintf(stdout, "test_init_sends_handle_as_controller_id\n");
	EXPECT(lfd >= 0);
	memset(&peer, 0, sizeof(peer));
	peer.listen_fd = lfd;
	peer.chunk = 1;
	EXPECT(run_init_against_peer(&peer, port) == 0);
	EXPECT(strstr(peer.req, classic) != NULL);
	EXPECT(strstr(peer.req, fd) != NULL);
	EXPECT(strstr(peer.req, tx) != NULL);
	EXPECT(strstr(peer.req, rx) != NULL);
	EXPECT(strstr(peer.req, tx_fd) != NULL);
	/* AUTOSAR indexes 0 and 2 must not be sent as the EcuBus controller id. */
	EXPECT(strstr(peer.req, "\"controllerId\":0") == NULL);
	EXPECT(strstr(peer.req, "\"controllerId\":2") == NULL);
	EXPECT(strstr(peer.req, "\"method\":\"Can.Init\"") != NULL);
	close(lfd);
}

static void test_init_read_survives_eintr(void)
{
	struct rpc_peer peer;
	uint16 port = 0;
	int lfd = listen_loopback(&port);

	fprintf(stdout, "test_init_read_survives_eintr\n");
	EXPECT(lfd >= 0);
	can_rpc_host_test_reset();
	memset(&peer, 0, sizeof(peer));
	peer.listen_fd = lfd;
	peer.interrupt_read = 1;
	EXPECT(run_init_against_peer(&peer, port) == 0);
	EXPECT(peer.saw_eintr == 1);
	EXPECT(can_rpc_host_test_eintr_retries() > 0u);
	EXPECT(strstr(peer.req, "\"controllerId\":1") != NULL);
	close(lfd);
}

static void test_peer_drop_is_not_ok(void)
{
	struct rpc_peer peer;
	uint16 port = 0;
	int lfd = listen_loopback(&port);

	fprintf(stdout, "test_peer_drop_is_not_ok\n");
	EXPECT(lfd >= 0);
	memset(&peer, 0, sizeof(peer));
	peer.listen_fd = lfd;
	peer.drop = 1;
	EXPECT(run_init_against_peer(&peer, port) != 0);
	close(lfd);
}

static void test_parse_result_code(void)
{
	Std_ReturnType result = 0xFFu;
	const char *ok = "{\"jsonrpc\":\"2.0\",\"result\":{\"result\":\"E_OK\",\"resultCode\":0},\"id\":1}\n";
	const char *busy = "{\"jsonrpc\":\"2.0\",\"result\":{\"result\":\"CAN_BUSY\",\"resultCode\":2},\"id\":2}\n";
	const char *err = "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32602,\"message\":\"missing controllerId\"},\"id\":3}\n";

	fprintf(stdout, "test_parse_result_code\n");
	EXPECT(Can_RpcParseResultCode(ok, &result) == E_OK);
	EXPECT(result == E_OK);
	EXPECT(Can_RpcParseResultCode(busy, &result) == E_OK);
	EXPECT(result == CAN_BUSY);
	EXPECT(Can_RpcParseResultCode(err, &result) == E_NOT_OK);
	EXPECT(result == E_NOT_OK);
}

struct read_job {
	int fd;
	ssize_t n;
	char buf[16];
};

static void *read_thread(void *arg)
{
	struct read_job *job = (struct read_job *)arg;

	job->n = can_rpc_host_read(job->fd, job->buf, sizeof(job->buf));
	return NULL;
}

static void test_host_read_retries_eintr(void)
{
	int sv[2];
	pthread_t thread;
	struct read_job job;
	unsigned base;

	fprintf(stdout, "test_host_read_retries_eintr\n");
	EXPECT(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == 0);
	can_rpc_host_test_reset();
	memset(&job, 0, sizeof(job));
	job.fd = sv[0];
	job.n = -99;
	EXPECT(pthread_create(&thread, NULL, read_thread, &job) == 0);
	base = can_rpc_host_test_eintr_retries();
	EXPECT(wait_for_eintr(thread, base, 2000) == 0);
	EXPECT(write_all(sv[1], "ok\n", 3) == 0);
	EXPECT(pthread_join(thread, NULL) == 0);
	EXPECT(job.n == 3);
	EXPECT(memcmp(job.buf, "ok\n", 3) == 0);
	EXPECT(can_rpc_host_test_eintr_retries() > base);
	close(sv[0]);
	close(sv[1]);
}

static void test_host_read_eof(void)
{
	int sv[2];
	char buf[8];
	ssize_t n;

	fprintf(stdout, "test_host_read_eof\n");
	EXPECT(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == 0);
	close(sv[1]);
	n = can_rpc_host_read(sv[0], buf, sizeof(buf));
	EXPECT(n == 0);
	close(sv[0]);
}

struct write_job {
	int fd;
	ssize_t n;
	char *buf;
	size_t len;
};

static void *write_thread(void *arg)
{
	struct write_job *job = (struct write_job *)arg;

	job->n = can_rpc_host_write(job->fd, job->buf, job->len);
	return NULL;
}

static void test_host_write_finishes_across_eintr(void)
{
	int sv[2];
	int flags;
	pthread_t thread;
	struct write_job job;
	unsigned base;
	char *payload;
	char *got;
	size_t filled = 0u;
	size_t extra = 4096u;
	size_t off;
	const size_t chunk = 1024u;

	fprintf(stdout, "test_host_write_finishes_across_eintr\n");
	EXPECT(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == 0);
	flags = fcntl(sv[0], F_GETFL, 0);
	EXPECT(flags >= 0);
	EXPECT(fcntl(sv[0], F_SETFL, flags | O_NONBLOCK) == 0);
	payload = (char *)malloc(chunk);
	EXPECT(payload != NULL);
	memset(payload, 0xab, chunk);
	for (;;) {
		ssize_t n = write(sv[0], payload, chunk);
		if (n < 0) {
			EXPECT((errno == EAGAIN) || (errno == EWOULDBLOCK));
			break;
		}
		EXPECT(n > 0);
		filled += (size_t)n;
		if (filled > (8u * 1024u * 1024u)) {
			EXPECT(0);
			break;
		}
	}
	EXPECT(fcntl(sv[0], F_SETFL, flags) == 0);
	EXPECT(filled > 0u);

	payload = (char *)realloc(payload, extra);
	EXPECT(payload != NULL);
	memset(payload, 0xcd, extra);
	got = (char *)malloc(filled + extra);
	EXPECT(got != NULL);

	can_rpc_host_test_reset();
	memset(&job, 0, sizeof(job));
	job.fd = sv[0];
	job.buf = payload;
	job.len = extra;
	job.n = -99;
	EXPECT(pthread_create(&thread, NULL, write_thread, &job) == 0);
	base = can_rpc_host_test_eintr_retries();
	EXPECT(wait_for_eintr(thread, base, 2000) == 0);

	off = 0u;
	while (off < filled + extra) {
		ssize_t n = read(sv[1], got + off, (filled + extra) - off);
		if ((n < 0) && (errno == EINTR)) {
			continue;
		}
		EXPECT(n > 0);
		if (n <= 0) {
			break;
		}
		off += (size_t)n;
	}
	EXPECT(pthread_join(thread, NULL) == 0);
	EXPECT(job.n == (ssize_t)extra);
	EXPECT(off == filled + extra);
	EXPECT(memcmp(got + filled, payload, extra) == 0);
	EXPECT(can_rpc_host_test_eintr_retries() > base);

	free(payload);
	free(got);
	close(sv[0]);
	close(sv[1]);
}

static void test_connect_eisconn_is_success(void)
{
	uint16 port = 0;
	int lfd = listen_loopback(&port);
	int cfd;
	struct sockaddr_in addr;
	int rc;

	/*
	 * A tick during connect(2) can leave the TCP handshake finished and the
	 * retry returning EISCONN. That must not fail Can_RpcConnect.
	 */
	fprintf(stdout, "test_connect_eisconn_is_success\n");
	EXPECT(lfd >= 0);
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	EXPECT(cfd >= 0);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(port);
	EXPECT(connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
	rc = can_rpc_host_connect(cfd, (struct sockaddr *)&addr, sizeof(addr));
	EXPECT(rc == 0);
	close(cfd);
	close(lfd);
}

static void test_close_live_fd(void)
{
	int sv[2];

	fprintf(stdout, "test_close_live_fd\n");
	EXPECT(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == 0);
	EXPECT(can_rpc_host_close(sv[0]) == 0);
	EXPECT(can_rpc_host_close(sv[0]) < 0);
	EXPECT(errno == EBADF);
	close(sv[1]);
}

int main(void)
{
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	EXPECT(pthread_sigmask(SIG_UNBLOCK, &set, NULL) == 0);
	arm_sigusr1();

	test_parse_result_code();
	test_init_sends_handle_as_controller_id();
	test_init_read_survives_eintr();
	test_peer_drop_is_not_ok();
	test_host_read_retries_eintr();
	test_host_read_eof();
	test_host_write_finishes_across_eintr();
	test_connect_eisconn_is_success();
	test_close_live_fd();

	if (g_failures != 0) {
		fprintf(stderr, "%d failure(s)\n", g_failures);
		return 1;
	}
	fprintf(stdout, "all can rpc transport tests passed\n");
	return 0;
}
