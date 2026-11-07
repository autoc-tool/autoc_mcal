#ifndef CAN_RPC_HOST_H
#define CAN_RPC_HOST_H
#ifndef _WIN32
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>
int can_rpc_host_socket(int domain, int type, int protocol);
int can_rpc_host_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int can_rpc_host_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
int can_rpc_host_close(int fd);
ssize_t can_rpc_host_read(int fd, void *buf, size_t count);
ssize_t can_rpc_host_write(int fd, const void *buf, size_t count);
ssize_t can_rpc_host_send(int sockfd, const void *buf, size_t len, int flags);
ssize_t can_rpc_host_recv(int sockfd, void *buf, size_t len, int flags);
#endif
#endif
