#ifndef __FT_HOOK_H
#define __FT_HOOK_H

#include <stdint.h>

#define SOCKET 0
#define CLOSE 1
#define CONNECT 2
#define READ 3
#define WRITE 4
#define SENDTO 5
#define RECVFROM 6
#define RECV 7
#define SEND 8
#define SETSOCKOPT 9
#define IOCTL 10
#define FCNTL 11
#define LISTEN 12
#define BIND 13
#define ACCEPT 14
#define SELECT 15
#define GETSOCKOPT 16

#define MAX_BUFF 2048

struct ft_ds_form
{
    int flag;
    int form;
};
struct ft_ds_socket
{
    int flag;
    int ret;
    int doamin;
    int type;
    int protocol;
};
struct ft_ds_connect
{
    int flag;
    int ret;
    int fd;
    struct sockaddr_in address;
    socklen_t addrlen_len;
};
struct ft_ds_bind
{
    int flag;
    int ret;
    int fd;
    struct sockaddr_in addr;
    socklen_t addrlen;
};
struct ft_ds_listen
{
    int flag;
    int ret;
    int fd;
    int backlog;
};
struct ft_ds_accept
{
    int flag;
    int ret;
    int fd;
    struct sockaddr_in addr;
    socklen_t addrlen;
};
struct ft_ds_write
{
    int flag;
    int ret;
    int fd;
    char buf[MAX_BUFF];
    size_t nbyte;
};
struct ft_ds_read
{
    int flag;
    int ret;
    int fd;
    char buf[MAX_BUFF];
    size_t nbyte;
};
struct ft_ds_send
{
    int flag;
    int ret;
    int fd;
    char buf[MAX_BUFF];
    size_t nbyte;
    int flags;
};
struct ft_ds_recv
{
    int flag;
    int ret;
    int fd;
    char buffer[MAX_BUFF];
    size_t length;
    int flags;
};
struct ft_ds_sendto
{
    int flag;
    int ret;
    int fd;
    char message[MAX_BUFF];
    size_t length;
    int flags;
    struct sockaddr_in dest_addr;
    socklen_t dest_len;
};
struct ft_ds_recvfrom
{
    int flag;
    int ret;
    int fd;
    char buffer[MAX_BUFF];
    size_t length;
    int flags;
    struct sockaddr_in address;
    socklen_t address_len;
};
struct ft_ds_close
{
    int flag;
    int ret;
    int fd;
};
struct ft_ds_select
{
    int flag;
    int ret;
    int nfds;
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
    struct timeval timeout;
};
struct ft_ds_sockopt
{
    int flag;
    int ret;
    int fd;
    int level;
    int option_name;
    char option_value[MAX_BUFF];
    socklen_t option_len;
};


typedef struct ft_msg_talk
{
    struct ft_ds_form ds_form;
    struct ft_ds_socket ds_socket;
    struct ft_ds_connect ds_connect;
    struct ft_ds_bind ds_bind;
    struct ft_ds_listen ds_listen;
    struct ft_ds_accept ds_accept;
    struct ft_ds_write ds_write;
    struct ft_ds_read ds_read;
    struct ft_ds_send ds_send;
    struct ft_ds_recv ds_recv;
    struct ft_ds_sendto ds_sendto;
    struct ft_ds_recvfrom ds_recvfrom;
    struct ft_ds_close ds_close;
    struct ft_ds_select ds_select;
    struct ft_ds_sockopt ds_setsockopt;
    struct ft_ds_sockopt ds_getsockopt;
} FMT;

int ft_hook_socket(int domain, int type, int protocol);
int ft_hook_close(int fd);
int ft_hook_connect(int fd, const struct sockaddr *address, socklen_t addrlen_len);

ssize_t ft_hook_read(int fd, void *buf, size_t nbyte);
ssize_t ft_hook_write(int fd, const void *buf, size_t nbyte);
ssize_t ft_hook_sendto(int fd, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);
ssize_t ft_hook_recvfrom(int fd, void *buffer, size_t length, int flags, struct sockaddr *address, socklen_t *address_len);
ssize_t ft_hook_recv(int fd, void *buffer, size_t length, int flags);
ssize_t ft_hook_send(int fd, const void *buf, size_t nbyte, int flags);

int ft_hook_setsockopt(int fd, int level, int option_name, const void *option_value, socklen_t option_len);
int ft_hook_getsockopt(int fd, int level, int option_name, void *option_value, socklen_t *option_len);
int ft_hook_ioctl(int fd, int cmd, void *arg);
int ft_hook_fcntl(int fd, int cmd, void *arg);
int ft_hook_listen(int fd, int backlog);
int ft_hook_bind(int fd, const struct sockaddr *addr, socklen_t addrlen);
int ft_hook_accept(int fd, struct sockaddr *addr, socklen_t *addrlen);

int ft_hook_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

#endif
