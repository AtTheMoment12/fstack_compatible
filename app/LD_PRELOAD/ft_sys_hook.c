//#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "ft_sys_hook.h"
#include "ft_hook.h"

#define FT_HOOK_MAX_FD 65535 * 2
#define FT_FD_FLG_INUSE 0x1
#define FT_FD_FLG_UNBLOCK 0x2

typedef struct socket_hook_info
{
    int sock_flag;
    int read_timeout;
    int write_timeout;
} FtHookFd;

FtSyscallFuncTab g_ft_syscall_tab;
int g_ft_hook_flag;
int g_ft_ff_hook_flag;
static FtHookFd g_ft_hook_fd_tab[FT_HOOK_MAX_FD];

FtHookFd *ft_hook_find_fd(int fd)
{
    if ((fd < 0) || (fd >= FT_HOOK_MAX_FD))
    {
        return NULL;
    }

    FtHookFd *fd_info = &g_ft_hook_fd_tab[fd];
    if (!(fd_info->sock_flag & FT_FD_FLG_INUSE))
    {
        return NULL;
    }
    else
    {
        return fd_info;
    }
}

void ft_hook_new_fd(int fd)
{
    if ((fd < 0) || (fd >= FT_HOOK_MAX_FD))
    {
        return;
    }

    FtHookFd *fd_info = &g_ft_hook_fd_tab[fd];
    fd_info->sock_flag = FT_FD_FLG_INUSE;
    fd_info->read_timeout = 500;
    fd_info->write_timeout = 500;
}

void ft_hook_free_fd(int fd)
{
    if ((fd < 0) || (fd >= FT_HOOK_MAX_FD))
    {
        return;
    }

    FtHookFd *fd_info = &g_ft_hook_fd_tab[fd];
    fd_info->sock_flag = 0;
    fd_info->read_timeout = 0;
    fd_info->write_timeout = 0;
}

int ioctl(int fd, unsigned long cmd, ...)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);

    va_list ap;
    va_start(ap, cmd);
    void *arg = va_arg(ap, void *);
    va_end(ap);

    ft_hook_syscall(ioctl);
    FtHookFd *hook_fd = ft_hook_find_fd(fd);

    if (!ft_hook_active() || !hook_fd || !ft_ff_hook_active())
    {
        return ft_hook_ioctl(fd, cmd, arg);
    }

    if (cmd == FIONBIO)
    {
        int flags = (arg != NULL) ? *((int *)arg) : 0;
        if (flags != 0)
        {
            hook_fd->sock_flag |= FT_FD_FLG_UNBLOCK;
        }
    }

    return ft_hook_ioctl(fd, cmd, arg);
}

int socket(int domain, int type, int protocol)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);

    int fd = ft_hook_socket(domain, type, protocol);
    printf("socket fd : %d \n", fd);
    if (fd < 0)
    {
        return fd;
    }
    return fd;
}

int close(int fd)
{
    printf("[FT_HOOK] %s,%d fd:%d\n", __FUNCTION__, __LINE__, fd);
    usleep(1000);
    int ret = ft_hook_close(fd);
    printf("[FT_HOOK] %s,%d fd:%d ret:%d\n", __FUNCTION__, __LINE__, fd,ret);
    return ret;
}

int connect(int fd, const struct sockaddr *address, socklen_t address_len)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);
    int ret = ft_hook_connect(fd, address, address_len);
    printf("[FT_HOOK] %s,%d ret:%d\n", __FUNCTION__, __LINE__,ret);
    return ret;
}
ssize_t read(int fd, void *buf, size_t nbyte)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);
/*
    ft_hook_syscall(read);
    FtHookFd *hook_fd = ft_hook_find_fd(fd);
    if (!ft_hook_active() || !hook_fd || !ft_ff_hook_active())
    {
        return ft_real_func(read)(fd, buf, nbyte);
    }

    if (hook_fd->sock_flag & FT_FD_FLG_UNBLOCK)
    {
        return ft_hook_read(fd, buf, nbyte);
    }
    //标注MtFrame::read
    */
    return ft_hook_read(fd, buf, nbyte);
}

ssize_t write(int fd, const void *buf, size_t nbyte)
{
    //printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);
    /*
    ft_hook_syscall(write);
    FtHookFd *hook_fd = ft_hook_find_fd(fd);
    if (!ft_hook_active() || !hook_fd || !ft_ff_hook_active())
    {
        return ft_real_func(write)(fd, buf, nbyte);
    }

    if (hook_fd->sock_flag & FT_FD_FLG_UNBLOCK)
    {
        return ft_hook_write(fd, buf, nbyte);
    }
    //标注MtFrame::write
    */
    int ret = -1;
    ret = ft_hook_write(fd, buf, nbyte);
    if(ret == -1)
    {
        time_t start = time(NULL);
        while (ret == -1 && (time(NULL) - start )< 0.01)
        {
            ret = ft_hook_write(fd, buf, nbyte);
        }
        
    }
    //printf("[FT_HOOK] %s,%d ret:%d\n", __FUNCTION__, __LINE__,ret);
    return ret;
}

ssize_t sendto(int fd, const void *message, size_t length, int flags,
               const struct sockaddr *dest_addr, socklen_t dest_len)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);
    return ft_hook_sendto(fd, message, length, flags, dest_addr, dest_len);
}

ssize_t recvfrom(int fd, void *buffer, size_t length, int flags,
                 struct sockaddr *address, socklen_t *address_len)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);

    int ret = -1;
    ret = ft_hook_recvfrom(fd, buffer, length, flags, address, address_len);

    if (ret == -1)
    {
        time_t start = time(NULL);
        while (ret == -1 && (time(NULL) - start) < 2)
        {
            ret = ft_hook_recvfrom(fd, buffer, length, flags, address, address_len);
        }
    }
    return ret;
}

ssize_t recv(int fd, void *buffer, size_t length, int flags)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);

    int ret = -1;
    ret = ft_hook_recv(fd, buffer, length, flags);
    if (ret == -1)
    {
        time_t start = time(NULL);
        while (ret == -1 && (time(NULL) - start) < 2)
        {
            ret = ft_hook_recv(fd, buffer, length, flags);
        }
    }
    printf("[FT_HOOK] %s,%d ret:%d\n", __FUNCTION__, __LINE__,ret);
    return ret;
}

ssize_t send(int fd, const void *buf, size_t nbyte, int flags)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);
    ssize_t ret = -1;
    ret = ft_hook_send(fd, buf, nbyte, flags);
    if(ret == -1)
    {
        time_t start = time(NULL);
        while (ret == -1 && (time(NULL) - start) < 2)
        {
            ret = ft_hook_send(fd, buf, nbyte, flags);
        }
    }
    printf("[FT_HOOK] %s,%d ret:%d\n", __FUNCTION__, __LINE__,ret);
    return ret;
}

int getsockopt(int fd, int level, int option_name, void *option_value, socklen_t *option_len)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);
    return ft_hook_getsockopt(fd, level, option_name, option_value, option_len);
}

int setsockopt(int fd, int level, int option_name, const void *option_value, socklen_t option_len)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);

    /*
    ft_hook_syscall(setsockopt);
    FtHookFd *hook_fd = ft_hook_find_fd(fd);
    if (!ft_hook_active() || !hook_fd || !ft_ff_hook_active())
    {
        return ft_real_func(setsockopt)(fd, level, option_name, option_value, option_len);
    }

    if (SOL_SOCKET == level)
    {
        struct timeval *val = (struct timeval *)option_value;
        if (SO_RCVTIMEO == option_name)
        {
            hook_fd->read_timeout = val->tv_sec * 1000 + val->tv_usec / 1000;
        }
        else if (SO_SNDTIMEO == option_name)
        {
            hook_fd->write_timeout = val->tv_sec * 1000 + val->tv_usec / 1000;
        }
    }
    */
    return ft_hook_setsockopt(fd, level, option_name, option_value, option_len);
}



int fcntl(int fd, int cmd, ...)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);

    va_list ap;
    va_start(ap, cmd);
    void *arg = va_arg(ap, void *);
    va_end(ap);

    ft_hook_syscall(fcntl);
    FtHookFd *hook_fd = ft_hook_find_fd(fd);
    if (!ft_hook_active() || !hook_fd || !ft_ff_hook_active())
    {
        return ft_real_func(fcntl)(fd, cmd, arg);
    }

    if (cmd == F_SETFL)
    {
        va_start(ap, cmd);
        int flags = va_arg(ap, int);
        va_end(ap);

        if (flags & O_NONBLOCK)
        {
            hook_fd->sock_flag |= FT_FD_FLG_UNBLOCK;
        }
    }

    return ft_hook_fcntl(fd, cmd, arg);
}

int listen(int sockfd, int backlog)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);

    return ft_hook_listen(sockfd, backlog);
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);

    return ft_hook_bind(sockfd, addr, addrlen);
}

int accept(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);

    return ft_hook_accept(fd, addr, addrlen);
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    printf("[FT_HOOK] %s,%d\n", __FUNCTION__, __LINE__);
    return ft_hook_select(nfds, readfds, writefds, exceptfds, timeout);
}
