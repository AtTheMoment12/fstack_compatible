#ifndef _FT_SYS_HOOK___
#define _FT_SYS_HOOK___

//#define _GNU_SOURCE
#include <poll.h>
#include <dlfcn.h>
#include "ff_api.h"


typedef int (*func_socket)(int domain, int type, int protocol);
typedef int (*func_bind)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*func_listen)(int sockfd, int backlog);
typedef int (*func_close)(int fd);
typedef int (*func_connect)(int socket, const struct sockaddr *address, socklen_t address_len);
typedef int (*func_accept)(int socket, struct sockaddr *address, socklen_t *addrlen);
typedef ssize_t (*func_read)(int fildes, void *buf, size_t nbyte);
typedef ssize_t (*func_write)(int fildes, const void *buf, size_t nbyte);
typedef ssize_t (*func_sendto)(int socket, const void *message, size_t length, 
                        int flags, const struct sockaddr *dest_addr, socklen_t dest_len);
typedef ssize_t (*func_recvfrom)(int socket, void *buffer, size_t length,
	                    int flags, struct sockaddr *address, socklen_t *address_len);
typedef size_t (*func_send)(int socket, const void *buffer, size_t length, int flags);
typedef ssize_t (*func_recv)(int socket, void *buffer, size_t length, int flags);
typedef int (*func_select)(int nfds, fd_set *readfds, fd_set *writefds,
                        fd_set *exceptfds, struct timeval *timeout);
typedef int (*func_poll)(struct pollfd fds[], nfds_t nfds, int timeout);
//typedef int (*func_setsockopt)(int socket, int level, int option_name,
//			            const void *option_value, socklen_t option_len);                        
//typedef int (*func_getsockopt)(int socket, int level, int option_name,
//			            void *option_value, socklen_t option_len);
typedef int (*func_ioctl)(int fd, unsigned long cmd, ...);
typedef int (*func_fcntl)(int fd, int cmd, ...);

typedef unsigned int (*func_sleep)(unsigned int seconds);			            

typedef struct ft_syscall_func_tab
{
    func_socket             real_socket;
	func_bind               real_bind;
	func_listen             real_listen;
    func_close              real_close;
    func_connect            real_connect;
    func_read               real_read;
    func_write              real_write;
    func_sendto             real_sendto;
    func_recvfrom           real_recvfrom;
    func_send               real_send;
    func_recv               real_recv;
    //func_setsockopt         real_setsockopt;
    //func_getsockopt         real_getsockopt;
    func_fcntl              real_fcntl;
    func_ioctl              real_ioctl;
    
    func_sleep              real_sleep;
    func_select             real_select;
    func_poll               real_poll;

    func_accept             real_accept;
} FtSyscallFuncTab;

extern FtSyscallFuncTab g_ft_syscall_tab;
extern int              g_ft_hook_flag;
extern int              g_ft_ff_hook_flag;

#define ft_hook_syscall(name)do{if(!g_ft_syscall_tab.real_##name){g_ft_syscall_tab.real_##name = (func_##name)dlsym(RTLD_NEXT, #name);}}while(0)                                                                   

#define ft_real_func(name)  g_ft_syscall_tab.real_##name

#define ft_set_hook_func(name)  (g_ft_hook_flag = 1)
#define ft_unset_hook_flag()    (g_ft_hook_flag = 0)

#define ft_hook_active()        (g_ft_hook_flag == 1)

#define ft_ff_set_hook_flag()      (g_ft_ff_hook_flag = 1)
#define ft_ff_unset_hook_flag()    (g_ft_ff_hook_flag = 0)
#define ft_ff_hook_active()        (g_ft_ff_hook_flag == 1)

#endif