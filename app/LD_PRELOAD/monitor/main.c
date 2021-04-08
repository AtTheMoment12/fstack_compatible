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
#define SHM_HUGE_1GB 1<<21
int shmid, shmid_ret;
FMT *fmt_action, *fmtr_action;
int kq;
struct epoll_event ev;
struct epoll_event events[MAX_EVENTS];
int epfd, ep_status = 0, accept_status = 0;
int running = 1, udp_status = 0;
;

int ft_init(void)
{
    shmid = shmget((key_t)1000, sizeof(FMT), IPC_CREAT);
    //shmid = shmget((key_t)1000, sizeof(FMT), IPC_CREAT|SHM_HUGETLB|SHM_HUGE_1GB);
    if (shmid == -1)
    {
        perror("shmid shmget()");
    }
    shmid_ret = shmget((key_t)1001, sizeof(FMT), IPC_CREAT);
    //shmid_ret = shmget((key_t)1001, sizeof(FMT), IPC_CREAT|SHM_HUGETLB|SHM_HUGE_1GB);
    if (shmid_ret == -1)
    {
        perror("shmid_ret shmget()");
    }
    fmt_action = (FMT *)shmat(shmid, 0, 0);
    fmtr_action = (FMT *)shmat(shmid_ret, 0, 0);

    assert((epfd = ff_epoll_create(10)) > 0);
    return 0;
}

void service(void *arg)
{
    if (fmt_action->ds_form.flag == 1)
    {

        int ret;
        switch (fmt_action->ds_form.form)
        {
        case ACCEPT:
            while (1)
            {
                if (fmt_action->ds_accept.flag == 1)
                {
                    break;
                }
            }
            if (fmt_action->ds_accept.fd > 0)
            {
                ret = ff_accept(fmt_action->ds_accept.fd, (struct linux_sockaddr *)&(fmt_action->ds_accept.addr), &fmt_action->ds_accept.addrlen);
                fmt_action->ds_accept.flag = 0;
                // printf("ff_accept msg is [%s:%d]\n", inet_ntoa(fmt_action->ds_accept.addr.sin_addr), ntohs(fmt_action->ds_accept.addr.sin_port));
                fmtr_action->ds_accept.ret = ret;
                fmtr_action->ds_accept.flag = 1;
            }
            fmt_action->ds_form.flag = 0;
            break;
        case SEND:
            while (1)
            {
                if (fmt_action->ds_send.flag == 1)
                {
                    break;
                }
            }
            if (fmt_action->ds_send.fd > 0)
            {
                ret = ff_send(fmt_action->ds_send.fd, fmt_action->ds_send.buf, fmt_action->ds_send.nbyte, fmt_action->ds_send.flags);
                fmt_action->ds_send.flag = 0;

                //printf("ff_send buf:[%s]\n", fmt_action->ds_send.buf);
                fmtr_action->ds_send.ret = ret;
                fmtr_action->ds_send.flag = 1;
            }
            fmt_action->ds_form.flag = 0;
            break;
        case RECV:
            while (1)
            {
                if (fmt_action->ds_recv.flag == 1)
                {
                    break;
                }
            }
            if (fmt_action->ds_recv.fd > 0)
            {

                ret = ff_recv(fmt_action->ds_recv.fd, fmt_action->ds_recv.buffer, fmt_action->ds_recv.length, fmt_action->ds_recv.flags);

                fmt_action->ds_recv.flag = 0;
                // printf("ff_recv buf:[%s]\n", fmt_action->ds_recv.buffer);

                fmtr_action->ds_recv.fd = fmt_action->ds_recv.fd;
                strcpy(fmtr_action->ds_recv.buffer, fmt_action->ds_recv.buffer);
                fmtr_action->ds_recv.ret = ret;
                fmtr_action->ds_recv.flag = 1;
            }
            fmt_action->ds_form.flag = 0;
            break;
        case SENDTO:
            while (1)
            {
                if (fmt_action->ds_sendto.flag == 1)
                {
                    break;
                }
            }
            ret = ff_sendto(fmt_action->ds_sendto.fd, fmt_action->ds_sendto.message, fmt_action->ds_sendto.length, fmt_action->ds_sendto.flags, (const struct linux_sockaddr *)&(fmt_action->ds_sendto.dest_addr), fmt_action->ds_sendto.dest_len);
            fmt_action->ds_sendto.flag = 0;
            // printf("sentto msg is [%s] [%s,%d]\n", fmt_action->ds_sendto.message, inet_ntoa(fmt_action->ds_sendto.dest_addr.sin_addr), ntohs(fmt_action->ds_sendto.dest_addr.sin_port));
            fmtr_action->ds_sendto.fd = fmt_action->ds_sendto.fd;
            fmtr_action->ds_sendto.ret = ret;
            fmtr_action->ds_sendto.flag = 1;

            fmt_action->ds_form.flag = 0;
            break;
        case RECVFROM:
            while (1)
            {
                if (fmt_action->ds_recvfrom.flag == 1)
                {
                    break;
                }
            }
            ret = ff_recvfrom(fmt_action->ds_recvfrom.fd, fmt_action->ds_recvfrom.buffer, fmt_action->ds_recvfrom.length, fmt_action->ds_recvfrom.flags, (struct linux_sockaddr *)&(fmt_action->ds_recvfrom.address), &fmt_action->ds_recvfrom.address_len);
            fmt_action->ds_recvfrom.flag = 0;
            //printf("recvfrom msg is [%s] [%s,%d] ret:%d\n", fmt_action->ds_recvfrom.buffer, inet_ntoa(fmt_action->ds_recvfrom.address.sin_addr), ntohs(fmt_action->ds_recvfrom.address.sin_port), ret);
            fmtr_action->ds_recvfrom.ret = ret;
            strcpy(fmtr_action->ds_recvfrom.buffer, fmt_action->ds_recvfrom.buffer);
            fmtr_action->ds_recvfrom.address.sin_addr = fmt_action->ds_recvfrom.address.sin_addr;
            fmtr_action->ds_recvfrom.address.sin_family = fmt_action->ds_recvfrom.address.sin_family;
            fmtr_action->ds_recvfrom.address.sin_port = fmt_action->ds_recvfrom.address.sin_port;
            fmtr_action->ds_recvfrom.flag = 1;

			fmt_action->ds_form.flag = 0;
            break;
        case WRITE:
            while (1)
            {
                if (fmt_action->ds_write.flag == 1)
                {
                    break;
                }
            }
            //printf("ff_write buf:[%s]\n",fmt_action->ds_write.buf);
            ret = ff_write(fmt_action->ds_write.fd, fmt_action->ds_write.buf, fmt_action->ds_write.nbyte);
            fmt_action->ds_write.flag = 0;

            fmtr_action->ds_write.ret = ret;
            fmtr_action->ds_write.flag = 1;

            fmt_action->ds_form.flag = 0;
            break;
        case READ:
            while (1)
            {
                if (fmt_action->ds_read.flag == 1)
                {
                    break;
                }
            }
            ret = ff_read(fmt_action->ds_read.fd, fmt_action->ds_read.buf, fmt_action->ds_read.nbyte);
            fmt_action->ds_read.flag = 0;

            fmtr_action->ds_read.ret = ret;
            strcpy(fmtr_action->ds_read.buf, fmt_action->ds_read.buf);
            fmtr_action->ds_read.flag = 1;

            fmt_action->ds_form.flag = 0;
            break;
        case GETSOCKOPT:
            printf("getsockopt \n");
            while (1)
            {
                if (fmt_action->ds_getsockopt.flag == 1)
                {
                    break;
                }
            }
            ret = ff_getsockopt(fmt_action->ds_getsockopt.fd, fmt_action->ds_getsockopt.level, fmt_action->ds_getsockopt.option_name, fmt_action->ds_getsockopt.option_value, &fmt_action->ds_getsockopt.option_len);
            fmt_action->ds_getsockopt.flag = 0;

            fmtr_action->ds_getsockopt.ret = ret;
            strcpy(fmtr_action->ds_getsockopt.option_value, fmt_action->ds_getsockopt.option_value);
            fmtr_action->ds_getsockopt.flag = 1;

            fmt_action->ds_form.flag = 0;
            break;
        case SELECT:
            printf("select \n");
            while (1)
            {
                if (fmt_action->ds_select.flag == 1)
                {
                    break;
                }
            }
            ret = ff_select(fmt_action->ds_select.nfds, &fmt_action->ds_select.readfds, &fmt_action->ds_select.writefds, &fmt_action->ds_select.exceptfds, &fmt_action->ds_select.timeout);
            fmt_action->ds_select.flag = 0;
            printf("ff_select ret:%d\n", ret);
            fmtr_action->ds_select.ret = ret;
            fmtr_action->ds_select.flag = 1;

            fmt_action->ds_form.flag = 0;
            break;
        case CLOSE:
            while (1)
            {
                if (fmt_action->ds_close.flag == 1)
                {
                    break;
                }
            }
            ret = ff_close(fmt_action->ds_close.fd);
            fmt_action->ds_close.flag = 0;
            // printf("ff_close fd:%d ret:%d\n", fmt_action->ds_close.fd, ret);
            fmtr_action->ds_close.ret = ret;
            fmtr_action->ds_close.flag = 1;

            fmt_action->ds_form.flag = 0;
            running = 1;
            udp_status = 0;
            break;

        default:
            break;
        }
    }
}

void service_epoll(void *arg)
{
    int nevents = ff_epoll_wait(epfd, events, MAX_EVENTS, 0);
    int i;
    if (udp_status == 1) //udp_server
    {
        for (i = 0; i < nevents; ++i)
        {
            if (events[i].events & EPOLLIN)
            {
                if (fmtr_action->ds_socket.ret != events[i].data.fd)
                    continue;
                int running_epoll = 1;
                while (running_epoll)
                {
                    if (fmt_action->ds_form.flag == 1)
                    {
                        int ret;
                        switch (fmt_action->ds_form.form)
                        {
                        case SENDTO:
                            while (1)
                            {
                                if (fmt_action->ds_sendto.flag == 1)
                                {
                                    break;
                                }
                            }
                            ret = ff_sendto(fmt_action->ds_sendto.fd, fmt_action->ds_sendto.message, fmt_action->ds_sendto.length, fmt_action->ds_sendto.flags, (const struct linux_sockaddr *)&(fmt_action->ds_sendto.dest_addr), fmt_action->ds_sendto.dest_len);
                            fmt_action->ds_sendto.flag = 0;
                            // printf("sentto msg is [%s] [%s,%d]\n", fmt_action->ds_sendto.message, inet_ntoa(fmt_action->ds_sendto.dest_addr.sin_addr), ntohs(fmt_action->ds_sendto.dest_addr.sin_port));
                            fmtr_action->ds_sendto.fd = fmt_action->ds_sendto.fd;
                            fmtr_action->ds_sendto.ret = ret;
                            fmtr_action->ds_sendto.flag = 1;

                            fmt_action->ds_form.flag = 0;
                            running_epoll = 0;
                            break;
                        case RECVFROM:
                            while (1)
                            {
                                if (fmt_action->ds_recvfrom.flag == 1)
                                {
                                    break;
                                }
                            }
                            ret = ff_recvfrom(fmt_action->ds_recvfrom.fd, fmt_action->ds_recvfrom.buffer, fmt_action->ds_recvfrom.length, fmt_action->ds_recvfrom.flags, (struct linux_sockaddr *)&(fmt_action->ds_recvfrom.address), &fmt_action->ds_recvfrom.address_len);
                            fmt_action->ds_recvfrom.flag = 0;
                            //printf("recvfrom msg is [%s] [%s,%d] ret:%d\n", fmt_action->ds_recvfrom.buffer, inet_ntoa(fmt_action->ds_recvfrom.address.sin_addr), ntohs(fmt_action->ds_recvfrom.address.sin_port), ret);

                            fmtr_action->ds_recvfrom.ret = ret;
                            strcpy(fmtr_action->ds_recvfrom.buffer, fmt_action->ds_recvfrom.buffer);
                            fmtr_action->ds_recvfrom.address.sin_addr = fmt_action->ds_recvfrom.address.sin_addr;
                            fmtr_action->ds_recvfrom.address.sin_family = fmt_action->ds_recvfrom.address.sin_family;
                            fmtr_action->ds_recvfrom.address.sin_port = fmt_action->ds_recvfrom.address.sin_port;
                            fmtr_action->ds_recvfrom.flag = 1;

                            fmt_action->ds_form.flag = 0;
                            break;

                        case CLOSE:
                            while (1)
                            {
                                if (fmt_action->ds_close.flag == 1)
                                {
                                    break;
                                }
                            }
                            ret = ff_close(fmt_action->ds_close.fd);
                            fmt_action->ds_close.flag = 0;
                            // printf("ff_close fd:%d ret:%d\n", fmt_action->ds_close.fd, ret);
                            fmtr_action->ds_close.ret = ret;
                            fmtr_action->ds_close.flag = 1;

                            fmt_action->ds_form.flag = 0;
                            running = 1;
                            running_epoll = 0;
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
    else // TCP server
    {
        for (i = 0; i < nevents; ++i)
        {
            // printf("socket_fd:%d\n", fmtr_action->ds_socket.ret);
            if (events[i].data.fd == fmtr_action->ds_socket.ret)
            {

                while (1)
                {
                    if (fmt_action->ds_accept.flag == 1 && fmt_action->ds_accept.fd > 0)
                    {
                        printf("accept while events[%d].data.fd:%d\n", i, events[i].data.fd);
                        int ret = ff_accept(fmt_action->ds_accept.fd, (struct linux_sockaddr *)&(fmt_action->ds_accept.addr), &fmt_action->ds_accept.addrlen);
                        if (ret < 0)
                        {
                            break;
                        }
                        fmtr_action->ds_accept.ret = ret;
                        fmtr_action->ds_accept.flag = 1;

                        fmt_action->ds_form.flag = 0;
                        ev.data.fd = fmtr_action->ds_accept.ret;
                        ev.events = EPOLLIN;

                        accept_status = 1;
                        if (ff_epoll_ctl(epfd, EPOLL_CTL_ADD, fmtr_action->ds_accept.ret, &ev) != 0)
                        {
                            printf("ff_epoll_ctl failed:%d, %s\n", errno, strerror(errno));
                            break;
                        }
                    }
                }
            }
            else
            {
                if (events[i].events & EPOLLERR)
                {
                    ff_epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    ff_close(events[i].data.fd);
                }
                else if (events[i].events & EPOLLIN)
                {
                    int running_epoll = 1;
                    while (running_epoll)
                    {
                        if (fmt_action->ds_form.flag == 1)
                        {
                            int ret;
                            switch (fmt_action->ds_form.form)
                            {
                            case SEND:
                                while (1)
                                {
                                    if (fmt_action->ds_send.flag == 1)
                                    {
                                        break;
                                    }
                                }
                                if (fmt_action->ds_send.fd > 0)
                                {
                                    ret = ff_send(fmt_action->ds_send.fd, fmt_action->ds_send.buf, fmt_action->ds_send.nbyte, fmt_action->ds_send.flags);
                                    fmt_action->ds_send.flag = 0;

                                    printf("ff_send buf:[%s]\n", fmt_action->ds_send.buf);
                                    fmtr_action->ds_send.ret = ret;
                                    fmtr_action->ds_send.flag = 1;
                                }
                                fmt_action->ds_form.flag = 0;
                                running_epoll = 0;
                                break;
                            case RECV:
                                printf("tcp server epoll ff_recv \n");
                                while (1)
                                {
                                    if (fmt_action->ds_recv.flag == 1)
                                    {
                                        break;
                                    }
                                }
                                if (fmt_action->ds_recv.fd > 0)
                                {
                                    ret = ff_recv(fmt_action->ds_recv.fd, fmt_action->ds_recv.buffer, fmt_action->ds_recv.length, fmt_action->ds_recv.flags);
                                    while (ret <= 0)
                                    {
                                        ret = ff_recv(fmt_action->ds_recv.fd, fmt_action->ds_recv.buffer, fmt_action->ds_recv.length, fmt_action->ds_recv.flags);
                                    }
                                    fmt_action->ds_recv.flag = 0;
                                    printf("ff_recv buf:[%s]  ret:%d\n", fmt_action->ds_recv.buffer, ret);
                                    fmtr_action->ds_recv.fd = fmt_action->ds_recv.fd;
                                    strcpy(fmtr_action->ds_recv.buffer, fmt_action->ds_recv.buffer);
                                    fmtr_action->ds_recv.ret = ret;
                                    fmtr_action->ds_recv.flag = 1;
                                }
                                fmt_action->ds_form.flag = 0;
                                break;
                            case WRITE:
                                while (1)
                                {
                                    if (fmt_action->ds_write.flag == 1)
                                    {
                                        break;
                                    }
                                }
                                //printf("ff_write buf:[%s]\n",fmt_action->ds_write.buf);
                                ret = ff_write(fmt_action->ds_write.fd, fmt_action->ds_write.buf, fmt_action->ds_write.nbyte);
                                fmt_action->ds_write.flag = 0;

                                fmtr_action->ds_write.ret = ret;
                                fmtr_action->ds_write.flag = 1;

                                fmt_action->ds_form.flag = 0;
                                break;
                            case READ:
                                while (1)
                                {
                                    if (fmt_action->ds_read.flag == 1)
                                    {
                                        break;
                                    }
                                }
                                ret = ff_read(fmt_action->ds_read.fd, fmt_action->ds_read.buf, fmt_action->ds_read.nbyte);
                                fmt_action->ds_read.flag = 0;

                                fmtr_action->ds_read.ret = ret;
                                strcpy(fmtr_action->ds_read.buf, fmt_action->ds_read.buf);
                                fmtr_action->ds_read.flag = 1;

                                fmt_action->ds_form.flag = 0;
                                break;
                            case GETSOCKOPT:
                                printf("getsockopt \n");
                                while (1)
                                {
                                    if (fmt_action->ds_getsockopt.flag == 1)
                                    {
                                        break;
                                    }
                                }
                                ret = ff_getsockopt(fmt_action->ds_getsockopt.fd, fmt_action->ds_getsockopt.level, fmt_action->ds_getsockopt.option_name, fmt_action->ds_getsockopt.option_value, &fmt_action->ds_getsockopt.option_len);
                                fmt_action->ds_getsockopt.flag = 0;

                                fmtr_action->ds_getsockopt.ret = ret;
                                strcpy(fmtr_action->ds_getsockopt.option_value, fmt_action->ds_getsockopt.option_value);
                                fmtr_action->ds_getsockopt.flag = 1;

                                fmt_action->ds_form.flag = 0;
                                break;
                            case SELECT:
                                printf("select \n");
                                while (1)
                                {
                                    if (fmt_action->ds_select.flag == 1)
                                    {
                                        break;
                                    }
                                }
                                ret = ff_select(fmt_action->ds_select.nfds, &fmt_action->ds_select.readfds, &fmt_action->ds_select.writefds, &fmt_action->ds_select.exceptfds, &fmt_action->ds_select.timeout);
                                fmt_action->ds_select.flag = 0;
                                printf("ff_select ret:%d\n", ret);
                                fmtr_action->ds_select.ret = ret;
                                fmtr_action->ds_select.flag = 1;

                                fmt_action->ds_form.flag = 0;
                                break;
                            case CLOSE:
                                while (1)
                                {
                                    if (fmt_action->ds_close.flag == 1)
                                    {
                                        break;
                                    }
                                }
                                ret = ff_close(fmt_action->ds_close.fd);
                                fmt_action->ds_close.flag = 0;

                                fmtr_action->ds_close.ret = ret;
                                fmtr_action->ds_close.flag = 1;

                                fmt_action->ds_form.flag = 0;
                                break;

                            default:
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void preparatory_work(void *arg)
{
    while (running)
    {
        if (fmt_action->ds_form.flag == 1)
        {
            int ret;
            switch (fmt_action->ds_form.form)
            {
            case SOCKET:
                while (1)
                {
                    if (fmt_action->ds_socket.flag == 1)
                    {
                        break;
                    }
                }
                ret = ff_socket(fmt_action->ds_socket.doamin, fmt_action->ds_socket.type, fmt_action->ds_socket.protocol);
                fmt_action->ds_socket.flag = 0;

                fmtr_action->ds_socket.ret = ret;
                fmtr_action->ds_socket.flag = 1;
                if (fmt_action->ds_socket.type == SOCK_DGRAM)
                    udp_status = 1;
                fmt_action->ds_form.flag = 0;
                fmtr_action->ds_socket.flag = 1;
                break;
            case CONNECT:
                while (1)
                {
                    if (fmt_action->ds_connect.flag == 1)
                    {
                        break;
                    }
                }
                if (fmt_action->ds_connect.fd > 0)
                {
                    int on = 1;
                    ff_ioctl(fmt_action->ds_connect.fd, 0x5421, &on);
                    ret = ff_connect(fmt_action->ds_connect.fd, (const struct linux_sockaddr *)&(fmt_action->ds_connect.address), sizeof(fmt_action->ds_connect.address));
                    fmt_action->ds_connect.flag = 0;
                    ret = 0;
                    fmtr_action->ds_connect.flag = 1;
                    fmtr_action->ds_connect.ret = ret;
                }
                fmt_action->ds_form.flag = 0;
                if (ret == 0)
                    running = 0;
                break;
            case BIND:
                while (1)
                {
                    if (fmt_action->ds_bind.flag == 1)
                    {
                        break;
                    }
                }
                if (fmt_action->ds_bind.fd > 0)
                {
                    ret = ff_bind(fmt_action->ds_bind.fd, (struct linux_sockaddr *)&(fmt_action->ds_bind.addr), sizeof(fmt_action->ds_bind.addr));
                    fmt_action->ds_bind.flag = 0;

                    fmtr_action->ds_bind.ret = ret;
                    fmtr_action->ds_bind.flag = 1;
                }
                if (fmt_action->ds_socket.type == SOCK_DGRAM)
                {
                    running = 0;
                    ep_status = 1;
                    ff_fcntl(fmt_action->ds_bind.fd, F_SETFL, ff_fcntl(fmt_action->ds_bind.fd, F_GETFD, 0) | O_NONBLOCK);
                    memset(&ev, 0, sizeof(ev));
                    ev.data.fd = fmt_action->ds_bind.fd;
                    ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
                    ff_epoll_ctl(epfd, EPOLL_CTL_ADD, fmt_action->ds_bind.fd, &ev);
                }
                fmt_action->ds_form.flag = 0;
                break;
            case LISTEN:
                while (1)
                {
                    if (fmt_action->ds_listen.flag == 1)
                    {
                        break;
                    }
                }
                if (fmt_action->ds_listen.fd > 0)
                {
                    ret = ff_listen(fmt_action->ds_listen.fd, fmt_action->ds_listen.backlog);
                    fmt_action->ds_listen.flag = 0;

                    fmtr_action->ds_listen.ret = ret;
                    fmtr_action->ds_listen.flag = 1;

                    ff_fcntl(fmtr_action->ds_socket.ret, F_SETFL, ff_fcntl(fmtr_action->ds_socket.ret, F_GETFD, 0) | O_NONBLOCK);
                    memset(&ev, 0, sizeof(ev));
                    ev.data.fd = fmtr_action->ds_socket.ret;
                    ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
                    ff_epoll_ctl(epfd, EPOLL_CTL_ADD, fmtr_action->ds_socket.ret, &ev);
                    ep_status = 1;
                }
                fmt_action->ds_form.flag = 0;
                if (ret == 0)
                    running = 0;
                break;

            case CLOSE:
                printf("close flag:%d\n", fmt_action->ds_close.flag);
                while (1)
                {
                    if (fmt_action->ds_close.flag == 1)
                    {
                        break;
                    }
                }
                ret = ff_close(fmt_action->ds_close.fd);
                printf("fd:%d ret:%d\n", fmt_action->ds_close.fd, ret);
                fmt_action->ds_close.flag = 0;

                fmtr_action->ds_close.ret = ret;
                fmtr_action->ds_close.flag = 1;

                fmt_action->ds_form.flag = 0;
                running = 1;
                break;
            case SELECT:
                printf("select \n");
                while (1)
                {
                    if (fmt_action->ds_select.flag == 1)
                    {
                        break;
                    }
                }
                ret = ff_select(fmt_action->ds_select.nfds, &fmt_action->ds_select.readfds, &fmt_action->ds_select.writefds, &fmt_action->ds_select.exceptfds, &fmt_action->ds_select.timeout);
                fmt_action->ds_select.flag = 0;

                fmtr_action->ds_select.ret = ret;
                fmtr_action->ds_select.flag = 1;

                fmt_action->ds_form.flag = 0;
                break;
            case SETSOCKOPT:
                printf("setsockopt \n");
                while (1)
                {
                    if (fmt_action->ds_setsockopt.flag == 1)
                    {
                        break;
                    }
                }
                ret = ff_setsockopt(fmt_action->ds_setsockopt.fd, fmt_action->ds_setsockopt.level, fmt_action->ds_setsockopt.option_name, fmt_action->ds_setsockopt.option_value, fmt_action->ds_setsockopt.option_len);
                fmt_action->ds_setsockopt.flag = 0;

                fmtr_action->ds_setsockopt.ret = ret;
                strcpy(fmtr_action->ds_setsockopt.option_value, fmt_action->ds_setsockopt.option_value);
                fmtr_action->ds_setsockopt.flag = 1;

                fmt_action->ds_form.flag = 0;
                break;
            case GETSOCKOPT:
                printf("getsockopt \n");
                while (1)
                {
                    if (fmt_action->ds_getsockopt.flag == 1)
                    {
                        break;
                    }
                }
                ret = ff_getsockopt(fmt_action->ds_getsockopt.fd, fmt_action->ds_getsockopt.level, fmt_action->ds_getsockopt.option_name, fmt_action->ds_getsockopt.option_value, &fmt_action->ds_getsockopt.option_len);
                fmt_action->ds_getsockopt.flag = 0;

                fmtr_action->ds_getsockopt.ret = ret;
                strcpy(fmtr_action->ds_getsockopt.option_value, fmt_action->ds_getsockopt.option_value);
                fmtr_action->ds_getsockopt.flag = 1;

                fmt_action->ds_form.flag = 0;
                break;

            default:
                break;
            }
        }
        if (fmt_action->ds_form.flag == 1 && udp_status == 1 && running == 1)
        {
            running = 0;
        }
    }
}

int loop(void *arg)
{
    preparatory_work(arg);
    if (ep_status == 0)
    {
        service(arg);
    }
    else
    {
        service_epoll(arg);
    }
}

int main(int argc, char *argv[])
{
    errno = 0;
    ff_init(argc, argv);
    ft_init();

    // printf("epfd:%d sockfd:%d sockfd:%d socket fd:%d \n",epfd,events[0].data.fd,events[1].data.fd,fmtr_action->ds_socket.ret);

    ff_run(loop, argv);
    return 0;
}
