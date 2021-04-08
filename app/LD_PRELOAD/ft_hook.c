#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sched.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>

#include "ff_api.h"
#include "ft_sys_hook.h"
#include "ft_hook.h"
#define SHM_HUGE_1GB 1<<21
int shmid, shmid_ret;
FMT *fmt_action, *fmtr_action;
int ft_init()
{
	printf("[FT_HOOK] %s,%d\n",__FUNCTION__,__LINE__);
	shmid = shmget((key_t)1000, sizeof(FMT), 0666 | IPC_CREAT);
	//shmid = shmget((key_t)1000, sizeof(FMT), 0666 | IPC_CREAT | SHM_HUGETLB | SHM_HUGE_1GB);
	if (shmid == -1)
	{
		perror("shmid shmget()");
	}
	shmid_ret = shmget((key_t)1001, sizeof(FMT), 0666 | IPC_CREAT);
	//shmid_ret = shmget((key_t)1001, sizeof(FMT), 0666 | IPC_CREAT | SHM_HUGETLB | SHM_HUGE_1GB);
	if (shmid_ret == -1)
	{
		perror("shmid ret shmget()");
	}
	fmt_action = (FMT *)shmat(shmid, 0, 0);
	fmtr_action = (FMT *)shmat(shmid_ret, 0, 0);
	return 0;
}

int ft_hook_socket(int domain, int type, int protocol)
{
	if (shmid == 0)
	{
		ft_init();
	}
	/*
	if ((AF_INET != domain) || (SOCK_STREAM != type && SOCK_DGRAM != type))
	{
		return ft_real_func(socket)(domain, type, protocol);
	}
	*/
	while (1)
	{
		if(fmt_action->ds_form.flag == 0){
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = SOCKET;
			break;
		}
	}
	fmt_action->ds_socket.flag = 1;
	fmt_action->ds_socket.doamin = domain;
	fmt_action->ds_socket.type = type;
	fmt_action->ds_socket.protocol = protocol;
	
	while (1)
	{
		if (fmtr_action->ds_socket.flag == 1)
		{
			fmtr_action->ds_socket.flag = 0;
			return fmtr_action->ds_socket.ret;
		}
	}
}

int ft_hook_close(int fd)
{
	printf("ft_hook_close flag :%d\n",fmt_action->ds_form.flag);
	while (1)
	{
		if(fmt_action->ds_form.flag == 0){
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = CLOSE;
			break;
		}
	}
	fmt_action->ds_close.flag = 1;
	fmt_action->ds_close.fd = fd;

	while (1)
	{
		if(fmtr_action->ds_close.flag == 1)
		{
			fmtr_action->ds_close.flag = 0;
			printf("ff_close ret:%d\n",fmtr_action->ds_close.ret);
			return fmtr_action->ds_close.ret;
		}
	}
	
	
}

int ft_hook_connect(int fd, const struct sockaddr *address, socklen_t addrlen_len)
{
	while (1)
	{
		if (fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = CONNECT;
			break;
		}
	}
	fmt_action->ds_connect.flag = 1;
	fmt_action->ds_connect.fd = fd;
	fmt_action->ds_connect.addrlen_len = addrlen_len;
	struct sockaddr_in *ser = (struct sockaddr_in *)address;
	fmt_action->ds_connect.address.sin_addr = ser->sin_addr;
	fmt_action->ds_connect.address.sin_family = ser->sin_family;
	fmt_action->ds_connect.address.sin_port = ser->sin_port;
	// printf("connect information is [%s:%d]\n", inet_ntoa(fmt_action->ds_connect.address.sin_addr), ntohs(fmt_action->ds_connect.address.sin_port));
	while (1)
	{
		if (fmtr_action->ds_connect.flag == 1)
		{
			fmtr_action->ds_connect.flag = 0;
			return fmtr_action->ds_connect.ret;
		}
	}
}

ssize_t ft_hook_read(int fd, void *buf, size_t nbyte)
{
	/*
	if (ff_fdisused(fd))
	{
		return ff_read(fd, buf, nbyte);
	}
	else
	{
		return ft_real_func(read)(fd, buf, nbyte);
	}
	*/
	while (1)
	{
		if(fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			break;
		}
	}
	fmt_action->ds_read.fd = fd;
	strcpy(fmt_action->ds_read.buf,buf);
	fmt_action->ds_read.nbyte = nbyte;
	fmt_action->ds_read.flag = 1;

	while (1)
	{
		if(fmtr_action->ds_read.flag == 1)
		{
			strcpy(buf,fmtr_action->ds_read.buf);
			fmtr_action->ds_read.flag = 0;
			return fmtr_action->ds_read.ret;
		}
	}
	
	
}

ssize_t ft_hook_write(int fd, const void *buf, size_t nbyte)
{
	/*
	if (ff_fdisused(fd))
	{
		return ff_write(fd, buf, nbyte);
	}
	else
	{
		return ft_real_func(write)(fd, buf, nbyte);
	}
	*/
	while (1)
	{
		if(fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = WRITE;
			break;
		}
	}
	fmt_action->ds_write.fd = fd;
	strcpy(fmt_action->ds_write.buf,buf);
	fmt_action->ds_write.nbyte = nbyte;
	fmt_action->ds_write.flag = 1;

	while (1)
	{
		if(fmtr_action->ds_write.flag == 1)
		{
			fmtr_action->ds_write.flag = 0;
			return fmtr_action->ds_write.ret;
		}
	}
	

	
}
ssize_t ft_hook_sendto(int fd, const void *message, size_t length, int flags,
					   const struct sockaddr *dest_addr, socklen_t dest_len)
{
	while (1)
	{
		if (fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = SENDTO;
			break;
		}
	}
	fmt_action->ds_sendto.fd = fd;
	strcpy(fmt_action->ds_sendto.message,message);
	fmt_action->ds_sendto.length = length;
	fmt_action->ds_sendto.flags = flags;
	struct sockaddr_in *ser = (struct sockaddr_in *)dest_addr;
	fmt_action->ds_sendto.dest_addr.sin_addr = ser->sin_addr;
	fmt_action->ds_sendto.dest_addr.sin_family = ser->sin_family;
	fmt_action->ds_sendto.dest_addr.sin_port = ser->sin_port;
	fmt_action->ds_sendto.dest_len = dest_len;
	fmt_action->ds_sendto.flag = 1;
	// printf("sentto msg is [%s] [%s,%d]\n",fmt_action->ds_sendto.message,inet_ntoa(fmt_action->ds_sendto.dest_addr.sin_addr),ntohs(fmt_action->ds_sendto.dest_addr.sin_port));
	while (1)
	{
		if (fmtr_action->ds_sendto.flag == 1)
		{
			fmtr_action->ds_sendto.flag = 0;
			return fmtr_action->ds_sendto.ret;
		}
	}
}
ssize_t ft_hook_recvfrom(int fd, void *buffer, size_t length, int flags,
						 struct sockaddr *address, socklen_t *address_len)
{
	while (1)
	{
		if (fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = RECVFROM;
			break;
		}
	}
	fmt_action->ds_recvfrom.fd = fd;
	strcpy(fmt_action->ds_recvfrom.buffer,buffer);
	fmt_action->ds_recvfrom.length = length;
	fmt_action->ds_recvfrom.flags = flags;
	struct sockaddr_in *ser = (struct sockaddr_in *)address;
	fmt_action->ds_recvfrom.address.sin_addr = ser->sin_addr;
	fmt_action->ds_recvfrom.address.sin_family = ser->sin_family;
	fmt_action->ds_recvfrom.address.sin_port = ser->sin_port;
	int len = sizeof(fmt_action->ds_recvfrom.address);
	fmt_action->ds_recvfrom.address_len = len;
	fmt_action->ds_recvfrom.flag = 1;
	while ((1))
	{
		if (fmtr_action->ds_recvfrom.flag == 1)
		{
			strcpy(buffer, fmtr_action->ds_recvfrom.buffer);
			ser->sin_addr = fmtr_action->ds_recvfrom.address.sin_addr;
			ser->sin_family = fmtr_action->ds_recvfrom.address.sin_family;
			ser->sin_port = fmtr_action->ds_recvfrom.address.sin_port;
			address = (struct sockaddr *)&ser;
			fmtr_action->ds_recvfrom.flag = 0;
			//printf("recvfrom msg is [%s] [%s,%d] ret:%d\n",buffer,inet_ntoa(fmt_action->ds_recvfrom.address.sin_addr),ntohs(fmt_action->ds_recvfrom.address.sin_port),fmtr_action->ds_recvfrom.ret);
			return fmtr_action->ds_recvfrom.ret;
		}
	}
}
ssize_t ft_hook_recv(int fd, void *buffer, size_t length, int flags)
{
	while (1)
	{
		if (fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = RECV;
			break;
		}
	}
	fmt_action->ds_recv.fd = fd;
	fmt_action->ds_recv.length = length;
	fmt_action->ds_recv.flags = flags;
	strcpy(fmt_action->ds_recv.buffer, buffer);
	fmt_action->ds_recv.flag = 1;
	while (1)
	{
		if (fmtr_action->ds_recv.flag == 1)
		{
			strcpy(buffer, fmtr_action->ds_recv.buffer);
			fmtr_action->ds_recv.flag = 0;
			return fmtr_action->ds_recv.ret;
		}
	}
}
ssize_t ft_hook_send(int fd, const void *buf, size_t nbyte, int flags)
{
	while (1)
	{
		if (fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = SEND;
			break;
		}
	}
	fmt_action->ds_send.fd = fd;
	fmt_action->ds_send.nbyte = nbyte;
	fmt_action->ds_send.flags = flags;
	strcpy(fmt_action->ds_send.buf, buf);
	fmt_action->ds_send.flag = 1;

	while (1)
	{
		if (fmtr_action->ds_send.flag == 1)
		{
			fmtr_action->ds_send.flag = 0;
			return fmtr_action->ds_send.ret;
		}
	}
}
int ft_hook_setsockopt(int fd, int level, int option_name, const void *option_value, socklen_t option_len)
{
	/*
	if (ff_fdisused(fd))
	{
		return ff_setsockopt(fd, level, option_name, option_value, option_len);
	}
	else
	{
		return ft_real_func(setsockopt)(fd, level, option_name, option_value, option_len);
	}
	*/
	while (1)
	{
		if(fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = SETSOCKOPT;
			break;
		}
	}
	fmt_action->ds_setsockopt.fd = fd;
	fmt_action->ds_setsockopt.level = level;
	fmt_action->ds_setsockopt.option_name = option_name;
	strcpy(fmt_action->ds_setsockopt.option_value,option_value);
	fmt_action->ds_setsockopt.option_len = option_len;
	fmt_action->ds_setsockopt.flag = 1;

	while (1)
	{
		if(fmtr_action->ds_setsockopt.flag == 1)
		{
			fmtr_action->ds_setsockopt.flag = 0;
			//strcpy(option_value,fmtr_action->ds_setsockopt.option_value);
			return fmtr_action->ds_setsockopt.ret;
		}
	}
	
}
int ft_hook_getsockopt(int fd, int level, int option_name, void *option_value, socklen_t *option_len)
{
	while (1)
	{
		if(fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = GETSOCKOPT;
			break;
		}
	}
	fmt_action->ds_getsockopt.fd = fd;
	fmt_action->ds_getsockopt.level = level;
	fmt_action->ds_getsockopt.option_name = option_name;
	strcpy(fmt_action->ds_getsockopt.option_value,option_value);
	fmt_action->ds_getsockopt.option_len = sizeof(fmt_action->ds_getsockopt.option_value);
	fmt_action->ds_getsockopt.flag = 1;

	while (1)
	{
		if(fmtr_action->ds_getsockopt.flag == 1)
		{
			fmtr_action->ds_getsockopt.flag = 0;
			strcpy(option_value, fmtr_action->ds_getsockopt.option_value);
			return fmtr_action->ds_getsockopt.ret;
		}
	}
	
	
}
int ft_hook_ioctl(int fd, int cmd, void *arg)
{

	if (ff_fdisused(fd))
	{
		return ff_ioctl(fd, cmd, arg);
	}
	else
	{
		return ft_real_func(ioctl)(fd, cmd, arg);
	}
}

int ft_hook_fcntl(int fd, int cmd, void *arg)
{

	if (ff_fdisused(fd))
	{
		return ff_fcntl(fd, cmd, arg);
	}
	else
	{
		return ft_real_func(fcntl)(fd, cmd, arg);
	}
}

int ft_hook_listen(int fd, int backlog)
{
	while (1)
	{
		if (fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = LISTEN;
			break;
		}
	}
	fmt_action->ds_listen.flag = 1;
	fmt_action->ds_listen.fd = fd;
	fmt_action->ds_listen.backlog = backlog;
	// printf("hook_listen fd:%d\n",fmt_action->ds_listen.fd);
	while (1)
	{
		if (fmtr_action->ds_listen.flag == 1)
		{
			fmtr_action->ds_listen.flag = 0;
			return fmtr_action->ds_listen.ret;
		}
	}
}

int ft_hook_bind(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
	while (1)
	{
		if (fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = BIND;
			break;
		}
	}
	fmt_action->ds_bind.flag = 1;
	fmt_action->ds_bind.fd = fd;
	fmt_action->ds_bind.addrlen = addrlen;
	struct sockaddr_in *ser = (struct sockaddr_in *)addr;
	fmt_action->ds_bind.addr.sin_addr = ser->sin_addr;
	fmt_action->ds_bind.addr.sin_family = ser->sin_family;
	fmt_action->ds_bind.addr.sin_port = ser->sin_port;
	// printf("bind msg is [%s:%d]\n", inet_ntoa(fmt_action->ds_bind.addr.sin_addr), ntohs(fmt_action->ds_bind.addr.sin_port));
	while (1)
	{
		if (fmtr_action->ds_bind.flag == 1)
		{
			fmtr_action->ds_bind.flag = 0;
			return fmtr_action->ds_bind.ret;
		}
	}
}

int ft_hook_accept(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
	while (1)
	{
		if (fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = ACCEPT;
			break;
		}
	}
	fmt_action->ds_accept.fd = fd;
	struct sockaddr_in *cli = (struct sockaddr_in *)addr;
	fmt_action->ds_accept.addr.sin_addr = cli->sin_addr;
	fmt_action->ds_accept.addr.sin_family = cli->sin_family;
	fmt_action->ds_accept.addr.sin_port = cli->sin_port;
	int len = sizeof(fmt_action->ds_accept.addr);
	fmt_action->ds_accept.addrlen = len;
	fmt_action->ds_accept.flag = 1;

	while (1)
	{
		if (fmtr_action->ds_accept.flag == 1)
		{
			fmtr_action->ds_accept.flag = 0;
			return fmtr_action->ds_accept.ret;
		}
	}
}


int ft_hook_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,struct timeval *timeout)
{
	while (1)
	{
		if(fmt_action->ds_form.flag == 0)
		{
			fmt_action->ds_form.flag = 1;
			fmt_action->ds_form.form = SELECT;
			break;
		}
	}
	
	fmt_action->ds_select.nfds = nfds;
	memcpy(&fmt_action->ds_select.readfds,&readfds,sizeof(fd_set));
	memcpy(&fmt_action->ds_select.writefds,&writefds,sizeof(fd_set));
	memcpy(&fmt_action->ds_select.exceptfds,&exceptfds,sizeof(fd_set));
	fmt_action->ds_select.timeout.tv_sec = timeout->tv_sec;
	fmt_action->ds_select.timeout.tv_usec = timeout->tv_usec;
	fmt_action->ds_select.flag = 1;

	while (1)
	{
		if(fmtr_action->ds_select.flag == 1)
		{
			fmtr_action->ds_select.flag = 0;
			return fmtr_action->ds_select.ret;
		}
	}
	

}
