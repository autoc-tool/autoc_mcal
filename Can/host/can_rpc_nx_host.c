#ifndef _WIN32
#include "can_rpc_host.h"
#include <sys/socket.h>
#include <unistd.h>

int can_rpc_host_socket(int domain, int type, int protocol)
{
	return socket(domain, type, protocol);
}
int can_rpc_host_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	return connect(sockfd, addr, addrlen);
}
int can_rpc_host_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
	return setsockopt(sockfd, level, optname, optval, optlen);
}
int can_rpc_host_close(int fd)
{
	return close(fd);
}
ssize_t can_rpc_host_read(int fd, void *buf, size_t count)
{
	return read(fd, buf, count);
}
ssize_t can_rpc_host_write(int fd, const void *buf, size_t count)
{
	return write(fd, buf, count);
}
ssize_t can_rpc_host_send(int sockfd, const void *buf, size_t len, int flags)
{
	return send(sockfd, buf, len, flags);
}
ssize_t can_rpc_host_recv(int sockfd, void *buf, size_t len, int flags)
{
	return recv(sockfd, buf, len, flags);
}
#endif
