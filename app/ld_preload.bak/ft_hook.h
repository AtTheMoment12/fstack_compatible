#ifndef __FT_HOOK_H
#define __FT_HOOK_H

#include <stdint.h>

#define SOCKET      0
#define CLOSE       1
#define CONNECT     2
#define READ        3
#define WRITE       4
#define SENDTO      5
#define RECVFROM    6
#define RECV        7
#define SEND        8
#define SETSOCKOPT  9
#define IOCTL       10
#define FCNTL       11
#define LISTEN      12
#define BIND        13
#define ACCEPT      14

/*
struct string_msg_file
{
    int fd;
    int flags;
    ssize_t nbyte;
    char buf[2048];

}__attribute__((packed));
*/
typedef struct ft_msg_talk
{
    int flag;
    int form;
    int domain;
    int type;
    int protocol;
    int fd;
    struct sockaddr_in address;
    socklen_t addrlen_len;
    ssize_t nbyte;
    //struct string_msg_file smf;
    //void *buf;
    char buf[2048];
    void *messege;
    ssize_t length;
    int flags;
    struct sockaddr *dest_addr;
    socklen_t dest_len;
    int backlog;
}FMT;
typedef struct ft_msg_talk_ret
{
    int fd;
    int flag;
    char recvbuff[2048];
}FMTR;

int ft_hook_socket(int domain, int type, int protocol);
int ft_hook_close(int fd);

int ft_hook_connect(int fd,const struct sockaddr *address, socklen_t addrlen_len);

ssize_t ft_hook_read(int fd, void *buf, size_t nbyte);

ssize_t ft_hook_write(int fd, const void *buf, size_t nbyte);
ssize_t ft_hook_sendto(int fd, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);
ssize_t ft_hook_recvfrom(int fd, void *buffer, size_t length, int flags, struct sockaddr *address, socklen_t *address_len);
ssize_t ft_hook_recv(int fd, void *buffer, size_t length, int flags);
ssize_t ft_hook_send(int fd, const void *buf, size_t nbyte, int flags);
int ft_hook_setsockopt(int fd, int level, int option_name, const void *option_value, socklen_t option_len);
int ft_hook_ioctl(int fd, int cmd, void *arg);

int ft_hook_fcntl(int fd, int cmd, void *arg);

int ft_hook_listen(int fd, int backlog);

int ft_hook_bind(int fd, const struct sockaddr *addr, socklen_t addrlen);
int ft_hook_accept(int fd, struct sockaddr *addr, socklen_t *addrlen);

#endif
