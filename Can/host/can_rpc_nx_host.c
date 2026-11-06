#ifndef _WIN32
#include "can_rpc_host.h"

#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

/*
 * NuttX sim runs this process on the host and delivers tick signals without
 * SA_RESTART. libc read/write/send/recv/connect then return EINTR (errno 4).
 * Can_RpcCall treats that as a dead socket, so Init never sees the reply
 * (captured on a real run: READ fd=3 -> -1 errno=4, can_ready=0).
 *
 * A tick that arrives after some bytes have been copied is not reported as
 * EINTR: write/send return a short count. Can_RpcCall requires the full
 * JSON-RPC line, so those calls keep going until the buffer is queued.
 * read/recv stay short-read APIs; the caller already accumulates until '\n'.
 *
 * close() is not retried. On Linux the descriptor is already released when
 * close returns EINTR, and a second close can hit an fd another thread reused.
 * connect() that lost the race with a tick can come back as EISCONN once the
 * handshake has finished; that is success.
 */

#if defined(CAN_RPC_HOST_TEST)
static unsigned can_rpc_host_eintr_retries;

void can_rpc_host_test_reset(void)
{
	__atomic_store_n(&can_rpc_host_eintr_retries, 0u, __ATOMIC_RELAXED);
}

unsigned can_rpc_host_test_eintr_retries(void)
{
	return __atomic_load_n(&can_rpc_host_eintr_retries, __ATOMIC_RELAXED);
}

static void note_eintr(void)
{
	(void)__atomic_fetch_add(&can_rpc_host_eintr_retries, 1u, __ATOMIC_RELAXED);
}
#else
static void note_eintr(void)
{
}
#endif

static int io_interrupted(ssize_t r)
{
	if ((r < 0) && (errno == EINTR)) {
		note_eintr();
		return 1;
	}
	return 0;
}

int can_rpc_host_socket(int domain, int type, int protocol)
{
	return socket(domain, type, protocol);
}

int can_rpc_host_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int r;

	do {
		r = connect(sockfd, addr, addrlen);
	} while (io_interrupted(r));
	if ((r < 0) && (errno == EISCONN)) {
		return 0;
	}
	return r;
}

int can_rpc_host_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
	return setsockopt(sockfd, level, optname, optval, optlen);
}

int can_rpc_host_close(int fd)
{
	int r = close(fd);

	if ((r < 0) && (errno == EINTR)) {
		return 0;
	}
	return r;
}

ssize_t can_rpc_host_read(int fd, void *buf, size_t count)
{
	ssize_t r;

	do {
		r = read(fd, buf, count);
	} while (io_interrupted(r));
	return r;
}

static ssize_t stream_all(ssize_t (*op)(int, const void *, size_t), int fd, const void *buf, size_t count)
{
	const unsigned char *p = (const unsigned char *)buf;
	size_t left = count;

	while (left > 0u) {
		ssize_t r = op(fd, p, left);

		if (io_interrupted(r)) {
			continue;
		}
		if (r < 0) {
			return -1;
		}
		if (r == 0) {
			break;
		}
		p += (size_t)r;
		left -= (size_t)r;
	}
	return (ssize_t)(count - left);
}

ssize_t can_rpc_host_write(int fd, const void *buf, size_t count)
{
	return stream_all(write, fd, buf, count);
}

ssize_t can_rpc_host_send(int sockfd, const void *buf, size_t len, int flags)
{
	const unsigned char *p = (const unsigned char *)buf;
	size_t left = len;

	while (left > 0u) {
		ssize_t r = send(sockfd, p, left, flags);

		if (io_interrupted(r)) {
			continue;
		}
		if (r < 0) {
			return -1;
		}
		if (r == 0) {
			break;
		}
		p += (size_t)r;
		left -= (size_t)r;
	}
	return (ssize_t)(len - left);
}

ssize_t can_rpc_host_recv(int sockfd, void *buf, size_t len, int flags)
{
	ssize_t r;

	do {
		r = recv(sockfd, buf, len, flags);
	} while (io_interrupted(r));
	return r;
}
#endif
