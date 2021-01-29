#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <assert.h>

#include "ff_api.h"
#include "ff_config.h"
#include "main.h"
#include "ff_epoll.h"
#include <fcntl.h>

#define MAX_EVENTS 512

int shmid;
int kq;
int shmid_socket;
int shmid_socket_ret;
int shmid_connect;
int shmid_connect_ret;
int shmid_send;
int shmid_send_ret;
int shmid_recv;
int shmid_recv_ret;
int shmid_bind;
int shmid_bind_ret;
int shmid_listen;
int shmid_listen_ret;
int shmid_accept;
int shmid_accept_ret;

struct epoll_event ev;
struct epoll_event events[MAX_EVENTS];
int epfd;

FMT *fmt_form, *fmt_socket, *fmt_connect, *fmt_bind, *fmt_listen, *fmt_accept, *fmt_send, *fmt_recv;
FMTR *fmtr_socket, *fmtr_connect, *fmtr_bind, *fmtr_listen, *fmtr_accept, *fmtr_send, *fmtr_recv;

int ft_init(void)
{
    shmid = shmget((key_t)1000, sizeof(FMT), IPC_CREAT);
    shmid_socket = shmget((key_t)(SOCKET + 1), sizeof(FMT), IPC_CREAT);
    shmid_socket_ret = shmget((key_t)(SOCKET + 15), sizeof(FMTR), IPC_CREAT);
    shmid_connect = shmget((key_t)(CONNECT + 1), sizeof(FMT), IPC_CREAT);
    shmid_connect_ret = shmget((key_t)(CONNECT + 15), sizeof(FMTR), IPC_CREAT);
    shmid_bind = shmget((key_t)(BIND + 1), sizeof(FMT), IPC_CREAT);
    shmid_bind_ret = shmget((key_t)(BIND + 15), sizeof(FMTR), IPC_CREAT);
    shmid_listen = shmget((key_t)(LISTEN + 1), sizeof(FMT), IPC_CREAT);
    shmid_listen_ret = shmget((key_t)(LISTEN + 15), sizeof(FMT), IPC_CREAT);
    shmid_send = shmget((key_t)(SEND + 1), sizeof(FMT), IPC_CREAT);
    shmid_send_ret = shmget((key_t)(SEND + 15), sizeof(FMTR), IPC_CREAT);
    shmid_recv = shmget((key_t)(RECV + 1), sizeof(FMT), IPC_CREAT);
    shmid_recv_ret = shmget((key_t)(RECV + 15), sizeof(FMTR), IPC_CREAT);

    fmt_form = (FMT *)shmat(shmid, 0, 0);
    fmt_socket = (FMT *)shmat(shmid_socket, 0, 0);
    fmt_connect = (FMT *)shmat(shmid_connect, 0, 0);
    fmt_bind = (FMT *)shmat(shmid_bind, 0, 0);
    fmt_listen = (FMT *)shmat(shmid_listen, 0, 0);
    fmt_accept = (FMT *)shmat(shmid_accept, 0, 0);
    fmt_send = (FMT *)shmat(shmid_send, 0, 0);
    fmt_recv = (FMT *)shmat(shmid_recv, 0, 0);

    fmtr_socket = (FMTR *)shmat(shmid_socket_ret, 0, 0);
    fmtr_connect = (FMTR *)shmat(shmid_connect_ret, 0, 0);
    fmtr_bind = (FMTR *)shmat(shmid_bind_ret, 0, 0);
    fmtr_listen = (FMTR *)shmat(shmid_listen_ret, 0, 0);
    fmtr_accept = (FMTR *)shmat(shmid_accept_ret, 0, 0);
    fmtr_send = (FMTR *)shmat(shmid_send_ret, 0, 0);
    fmtr_recv = (FMTR *)shmat(shmid_recv_ret, 0, 0);
    return 0;
}

void service(void *arg)
{
    int nevents = ff_epoll_wait(epfd, events, MAX_EVENTS, 0);
    int i;
    //for (i = 0; i < nevents; ++i)
    //{
    //    if(events[i].data.fd = fmt_socket->fd){

    //    }
    FMT *fmt_form = (FMT *)arg;
    if (fmt_form->flag == 1)
    {

        switch (fmt_form->form)
        {
            int fd;
        case SEND:
            printf("send fd:[%d] \n", fmt_send->fd);
            while (1)
            {
                if (fmt_send->flag == 1)
                {
                    break;
                }
            }
            if (fmt_send->flag == 1)
            {

                printf("write msg is [%lx] [%lx] [%lx] [%lx]\n", &fmt_send->fd, &fmt_send->buf, &fmt_send->nbyte, &fmt_send->flags);
                printf("write msg is [%d] [%s] [%d] [%d]\n", fmt_send->fd, fmt_send->buf, fmt_send->nbyte, fmt_send->flags);
                ff_send(fmt_send->fd, fmt_send->buf, fmt_send->nbyte, fmt_send->flags);

                fmt_send->flag = 0;
            }
            fmt_form->flag = 0;
            break;
        case RECV:
            printf("fd:[%d] \n", fmt_recv->fd);
            while (1)
            {
                if (fmt_recv->flag == 1)
                {
                    break;
                }
            }
            if (fmt_recv->flag == 1)
            {

                printf("monitor receive: %s %lx %d \n", fmt_recv->buf, &fmt_recv->buf, fmt_recv->length);
                ff_recv(fmt_recv->fd, fmt_recv->buf, fmt_recv->length, fmt_recv->flags);
                printf("monitor receive: %s %lx %d \n", fmt_recv->buf, &fmt_recv->buf, fmt_recv->length);

                fmtr_recv->fd = fmt_send->fd;
                strcpy(fmtr_recv->recvbuff, fmt_recv->buf);
                fmtr_recv->flag = 1;

                fmt_recv->flag = 0;
            }

            fmt_form->flag = 0;
            printf("receive msg: %s\n", fmtr_recv->recvbuff);
            break;
        case ACCEPT:
            while (1)
            {
                if (fmt_accept->flag == 1)
                {
                    break;
                }
            }
            if (fmt_accept->flag == 1)
            {
                fd = ff_accept(fmt_accept->fd, (struct linux_sockaddr *)&(fmt_accept->address), &fmt_accept->addrlen_len);
                fmt_accept->flag = 0;

                fmtr_accept->fd = fd;
                fmtr_accept->flag = 1;
            }
            fmt_form->flag = 0;
            break;
        default:
            break;
        }
    }
    //}
}

int loop(void *arg)
{
    service(arg);
}

int main(int argc, char *argv[])
{
    errno = 0;
    ff_init(argc, argv);
    ft_init();
    int running = 1;
    while (running)
    {
        if (fmt_form->flag == 1)
        {
            int fd;
            switch (fmt_form->form)
            {
            case SOCKET:
                printf("write msg is [%d] [%d] [%d] [%d]\n", fmt_socket->form, fmt_socket->domain, fmt_socket->type, fmt_socket->protocol);
                fd = ff_socket(fmt_socket->domain, fmt_socket->type, fmt_socket->protocol);
                fmtr_socket->fd = fd;
                fmtr_socket->flag = 1;
                printf("ret :%d %lx\n", fmtr_socket->fd, fmtr_socket->fd);
                fmt_form->flag = 0;
                break;
            case CONNECT:

                printf("fd:[%d] \n", fmt_connect->fd);
                while (1)
                {
                    if (fmt_connect->flag == 1)
                    {
                        break;
                    }
                }
                printf("fd:[%d] \n", fmt_connect->fd);
                if (fmt_connect->fd > 0)
                {
                    int on = 1;
                    ff_ioctl(fmt_connect->fd, 0x5421, &on);

                    printf("write msg is fd:[%d] [%s] [%d:%d] size:[%d]\n", fmt_connect->fd, inet_ntoa(fmt_connect->address.sin_addr),
                           fmt_connect->address.sin_addr.s_addr, ntohs(fmt_connect->address.sin_port), sizeof(fmt_connect->address));
                    fd = ff_connect(fmt_connect->fd, (const struct linux_sockaddr *)&(fmt_connect->address), sizeof(fmt_connect->address));
                    printf("retrun fd:%d\n", fd);
                    if (fd < 0 && errno != EINPROGRESS)
                        printf("conn failed, clientfd = %d,ret=%d,%d,%s\n", fmt_connect->fd, fd, errno, strerror(errno));
                    else
                        printf("conn suc\n");
                    fmt_connect->flag = 0;
                    fd = 0;
                    printf("retrun fd:%d\n", fd);
                    fmtr_connect->fd = fd;
                    fmtr_connect->flag = 1;
                    printf("ret :%d %lx \n", fmtr_connect->fd, fmtr_connect->fd);
                }
                fmt_form->flag = 0;
                if (fd == 0)
                    running = 0;
                break;
            case BIND:
                printf("fd:[%d]\n", fmt_bind->fd);
                while (1)
                {
                    if (fmt_bind->flag == 1)
                    {
                        break;
                    }
                }
                if (fmt_bind->fd > 0)
                {
                    fd = ff_bind(fmt_bind->fd, (struct linux_sockaddr *)&(fmt_bind->address), sizeof(fmt_bind->address));
                    printf("return fd :%d \n", fd);
                    fmt_bind->flag = 0;

                    fmtr_bind->fd = fd;
                    fmtr_bind->flag = 1;
                }
                fmt_form->flag = 0;
                break;
            case LISTEN:
                while (1)
                {
                    if (fmt_listen->flag == 1)
                    {
                        break;
                    }
                }
                if (fmt_listen->fd > 0)
                {
                    fd = ff_listen(fmt_listen->fd, fmt_listen->backlog);
                    printf("return fd :%d \n", fd);
                    fmt_listen->flag = 0;

                    fmtr_listen->fd = fd;
                    fmtr_listen->flag = 1;
                }
                printf("1\n");
                fmt_form->flag = 0;
                if (fd == 0)
                    running = 0;
            default:
                break;
            }
        }
    }
    /*
    assert((epfd = ff_epoll_create(0)) > 0);
    ev.data.fd = fmt_socket->fd;
    ev.events = EPOLLIN;

    fcntl(fmt_socket->fd, F_SETFL, fcntl(fmt_socket->fd, F_GETFD, 0) | O_NONBLOCK);
    ff_epoll_ctl(epfd, EPOLL_CTL_ADD, fmt_socket->fd, &ev);
    */
    printf("2\n");
    ff_run(loop, fmt_form);
    return 0;
}
