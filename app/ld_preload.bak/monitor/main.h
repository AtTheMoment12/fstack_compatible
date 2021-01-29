#ifndef __MAIN_H__
#define __MAIN_H__

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
    const struct sockaddr_in address;
    socklen_t addrlen_len;
    ssize_t nbyte;
    //struct string_msg_file smf;
    //void *buf;
    char buf[2048];
    const void *messege;
    ssize_t length;
    int flags;
    const struct sockaddr *dest_addr;
    socklen_t dest_len;
    int backlog;
}FMT;

typedef struct ft_msg_talk_ret
{
    int fd;
    int flag;
    char recvbuff[2048];
}FMTR;

#endif