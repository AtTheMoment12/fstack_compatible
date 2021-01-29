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
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include "ff_api.h"
#include "ft_sys_hook.h"
#include "ft_hook.h"
#include <arpa/inet.h>

int shmid;
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

FMT *fmt_form;
FMT *fmt_socket, *fmt_connect, *fmt_bind, *fmt_listen, *fmt_accept, *fmt_send, *fmt_recv;
FMTR *fmtr_socket, *fmtr_connect, *fmtr_bind, *fmtr_listen, *fmtr_accept, *fmtr_send, *fmtr_recv;

int ft_init()
{
	shmid = shmget((key_t)1000, sizeof(FMT), 0666 | IPC_CREAT);
	shmid_socket = shmget((key_t)(SOCKET + 1), sizeof(FMT), 0666 | IPC_CREAT);
	shmid_socket_ret = shmget((key_t)(SOCKET + 15), sizeof(FMTR), 0666 | IPC_CREAT);
	shmid_connect = shmget((key_t)(CONNECT + 1), sizeof(FMT), 0666 | IPC_CREAT);
	shmid_connect_ret = shmget((key_t)(CONNECT + 15), sizeof(FMTR), 0666 | IPC_CREAT);
	shmid_send = shmget((key_t)(SEND + 1), sizeof(FMT), 0666 | IPC_CREAT);
	shmid_send_ret = shmget((key_t)(SEND + 15), sizeof(FMTR), 0666 | IPC_CREAT);
	shmid_recv = shmget((key_t)(RECV + 1), sizeof(FMT), 0666 | IPC_CREAT);
	shmid_recv_ret = shmget((key_t)(RECV + 15), sizeof(FMTR), 0666 | IPC_CREAT);
	shmid_bind = shmget((key_t)(BIND + 1), sizeof(FMT), 0666 | IPC_CREAT);
	shmid_bind_ret = shmget((key_t)(BIND + 15), sizeof(FMTR), 0666 | IPC_CREAT);
	shmid_listen = shmget((key_t)(LISTEN + 1), sizeof(FMT), 0666 | IPC_CREAT);
	shmid_listen_ret = shmget((key_t)(LISTEN + 15), sizeof(FMTR), 0666 | IPC_CREAT);
	shmid_accept = shmget((key_t)(ACCEPT + 1), sizeof(FMT), 0666 | IPC_CREAT);
	if(shmid_accept == -1){
        perror("accept shmget()");
    }
	shmid_accept_ret = shmget((key_t)(ACCEPT + 1), sizeof(FMTR), 0666 | IPC_CREAT);
	if(shmid_accept_ret == -1){
        perror("accept ret shmget()");
    }

	fmt_form = (FMT *)shmat(shmid, 0, 0);
	fmt_socket = (FMT *)shmat(shmid_socket, 0, 0);
	fmt_connect = (FMT *)shmat(shmid_connect, 0, 0);
	fmt_send = (FMT *)shmat(shmid_send, 0, 0);
	fmt_recv = (FMT *)shmat(shmid_recv, 0, 0);
	fmt_bind = (FMT *)shmat(shmid_bind, 0, 0);
	fmt_listen = (FMT *)shmat(shmid_listen, 0, 0);
	fmt_accept = (FMT *)shmat(shmid_accept, 0, 0);

	fmtr_socket = (FMTR *)shmat(shmid_socket_ret, 0, 0);
	fmtr_connect = (FMTR *)shmat(shmid_connect_ret, 0, 0);
	fmtr_send = (FMTR *)shmat(shmid_send_ret, 0, 0);
	fmtr_recv = (FMTR *)shmat(shmid_recv_ret, 0, 0);
	fmtr_bind = (FMTR *)shmat(shmid_bind_ret, 0, 0);
	fmtr_listen = (FMTR *)shmat(shmid_listen_ret, 0, 0);
	fmtr_accept = (FMTR *)shmat(shmid_accept_ret, 0, 0);
	return 0;
}

int ft_hook_socket(int domain, int type, int protocol)
{
	printf("DEBUG ft_hook_socket start d\n");
	if (shmid_socket == 0)
	{
		ft_init();
	}

	if ((AF_INET != domain) || (SOCK_STREAM != type && SOCK_DGRAM != type))
	{
		return ft_real_func(socket)(domain, type, protocol);
	}
	fmt_form->flag = 1;
	fmt_form->form = SOCKET;

	fmt_socket->flag = 1;
	fmt_socket->form = SOCKET;
	fmt_socket->domain = domain;
	fmt_socket->type = type;
	fmt_socket->protocol = protocol;
	while (1)
	{
		if (fmtr_socket->flag == 1)
		{
			printf("ret:%d %lx\n", fmtr_socket->fd, fmtr_socket->fd);
			fmtr_socket->flag = 0;
			return fmtr_socket->fd;
		}
		//sleep(1);
	}

	//return ff_socket(domain, type, protocol);
}

int ft_hook_close(int fd)
{
	if (ff_fdisused(fd))
	{
		return ff_close(fd);
	}
	else
	{
		return ft_real_func(close)(fd);
	}
}

int ft_hook_connect(int fd, const struct sockaddr *address, socklen_t addrlen_len)
{

	printf("flag:%d fd:%d\n", fmt_form->flag, fd);
	int flag = 1;
	while (flag)
	{
		if (fmt_form->flag == 0)
		{
			fmt_form->flag = 1;
			fmt_form->form = CONNECT;
			flag = 0;
		}
	}
	fmt_connect->flag = 1;
	fmt_connect->form = CONNECT;
	fmt_connect->fd = fd;
	fmt_connect->addrlen_len = addrlen_len;

	struct sockaddr_in *ser = (struct sockaddr_in *)address;
	fmt_connect->address.sin_addr = ser->sin_addr;
	fmt_connect->address.sin_family = ser->sin_family;
	fmt_connect->address.sin_port = ser->sin_port;
	printf("write msg is [%s] [%d:%d]\n", inet_ntoa(fmt_connect->address.sin_addr), fmt_connect->address.sin_addr.s_addr, ntohs(fmt_connect->address.sin_port));

	while (1)
	{
		if (fmtr_connect->flag == 1)
		{
			printf("ret:%d %lx\n", fmtr_connect->fd, fmtr_connect->fd);
			fmtr_connect->flag = 0;
			return fmtr_connect->fd;
		}
	}
	if (ff_fdisused(fd))
	{
		return ff_connect(fd, (struct linux_sockaddr *)address, addrlen_len);
	}
	else
	{
		return ft_real_func(connect)(fd, address, addrlen_len);
	}
}

ssize_t ft_hook_read(int fd, void *buf, size_t nbyte)
{

	if (ff_fdisused(fd))
	{
		return ff_read(fd, buf, nbyte);
	}
	else
	{
		return ft_real_func(read)(fd, buf, nbyte);
	}
}

ssize_t ft_hook_write(int fd, const void *buf, size_t nbyte)
{

	if (ff_fdisused(fd))
	{
		return ff_write(fd, buf, nbyte);
	}
	else
	{
		return ft_real_func(write)(fd, buf, nbyte);
	}
}
ssize_t ft_hook_sendto(int fd, const void *message, size_t length, int flags,
					   const struct sockaddr *dest_addr, socklen_t dest_len)
{

	if (ff_fdisused(fd))
	{
		return ff_sendto(fd, message, length, flags, (struct linux_sockaddr *)dest_addr, dest_len);
	}
	else
	{
		return ft_real_func(sendto)(fd, message, length, flags, dest_addr, dest_len);
	}
}
ssize_t ft_hook_recvfrom(int fd, void *buffer, size_t length, int flags,
						 struct sockaddr *address, socklen_t *address_len)
{
	if (ff_fdisused(fd))
	{
		return ff_recvfrom(fd, buffer, length, flags, (struct linux_sockaddr *)address, address_len);
	}
	else
	{
		return ft_real_func(recvfrom)(fd, buffer, length, flags, address, address_len);
	}
}
ssize_t ft_hook_recv(int fd, void *buffer, size_t length, int flags)
{
	printf("flag:%d fd:%d\n", fmt_form->flag, fd);
	int flag = 1;
	while (flag)
	{
		if (fmt_form->flag == 0)
		{
			fmt_form->flag = 1;
			fmt_form->form = RECV;
			flag = 0;
		}
	}

	fmt_recv->fd = fd;
	fmt_recv->length = length;
	fmt_recv->flags = flags;
	strcpy(fmt_recv->buf, buffer);
	fmt_recv->flag = 1;
	int running = 1;
	while (running)
	{
		if (fmtr_recv->flag == 1)
		{
			strcpy(buffer, fmtr_recv->recvbuff);
			printf("get msg is [%s]\n", fmtr_recv->recvbuff);
			fmtr_recv->flag = 0;
			running = 0;
		}
	}
	/*
	if (ff_fdisused(fd)) {
		return ff_recv(fd, buffer, length, flags);
	} else {
        return ft_real_func(recv)(fd, buffer, length, flags);
	}
	*/
}
ssize_t ft_hook_send(int fd, const void *buf, size_t nbyte, int flags)
{
	printf("flag:%d fd:%d\n", fmt_form->flag, fd);
	int flag = 1;
	while (flag)
	{
		if (fmt_form->flag == 0)
		{
			fmt_form->flag = 1;
			fmt_form->form = SEND;
			flag = 0;
		}
	}

	fmt_send->fd = fd;
	fmt_send->nbyte = nbyte;
	fmt_send->flags = flags;
	strcpy(fmt_send->buf, buf);

	printf("source msg is fd:[%d] buf:[%s] nbyte:[%d] flags:[%d]\n", fd, buf, nbyte, flags);
	fmt_send->flag = 1;

	printf("write msg is [%lx] [%lx] [%lx] [%lx]\n", &fmt_send->fd, &fmt_send->buf, &fmt_send->nbyte, &fmt_send->flags);

	usleep(100);
	//printf("write msg is  [%lx] [%lx] [%d] [%s] [%d] [%d]\n",fmt_send->smf.buf, buf,fmt_send->smf.fd, fmt_send->smf.buf,fmt_send->smf.nbyte,fmt_send->smf.flags);
	return 0;
	/*
	if (ff_fdisused(fd)) {
		return ff_send(fd, buf, nbyte, flags);
	} else {
        return ft_real_func(send)(fd, buf, nbyte, flags);
	}*/
}
int ft_hook_setsockopt(int fd, int level, int option_name, const void *option_value, socklen_t option_len)
{
	if (ff_fdisused(fd))
	{
		return ff_setsockopt(fd, level, option_name, option_value, option_len);
	}
	else
	{
		return ft_real_func(setsockopt)(fd, level, option_name, option_value, option_len);
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
	printf("listen flag:%d fd:%d\n", fmt_form->flag, fd);
	int flag = 1;
	while (flag)
	{
		if (fmt_form->flag == 0)
		{
			fmt_form->flag = 1;
			fmt_form->form = LISTEN;
			flag = 0;
		}
	}
	fmt_listen->flag = 1;
	fmt_listen->form = LISTEN;
	fmt_listen->fd = fd;
	fmt_listen->backlog = backlog;
	while (1)
	{
		if (fmtr_listen->flag == 1)
		{
			fmtr_listen->flag = 0;
			printf("listen ret fd : %d\n",fmtr_listen->fd);
			return fmtr_listen->fd;
		}
	}
	if (ff_fdisused(fd))
	{
		return ff_listen(fd, backlog);
	}
	else
	{
		return ft_real_func(listen)(fd, backlog);
	}
}

int ft_hook_bind(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
	printf("flag:%d fd:%d\n", fmt_form->flag, fd);
	int flag = 1;
	while (flag)
	{
		if (fmt_form->flag == 0)
		{
			fmt_form->flag = 1;
			fmt_form->form = BIND;
			flag = 0;
		}
	}
	fmt_bind->flag = 1;
	fmt_bind->form = BIND;
	fmt_bind->fd = fd;
	fmt_bind->addrlen_len = addrlen;

	struct sockaddr_in *ser = (struct sockaddr_in *)addr;
	fmt_bind->address.sin_addr = ser->sin_addr;
	fmt_bind->address.sin_family = ser->sin_family;
	fmt_bind->address.sin_port = ser->sin_port;
	printf("write msg is [%s] [%d:%d]\n", inet_ntoa(fmt_bind->address.sin_addr), fmt_bind->address.sin_addr.s_addr, ntohs(fmt_bind->address.sin_port));

	while (1)
	{
		if (fmtr_bind->flag == 1)
		{
			printf("ret:%d %lx\n", fmtr_bind->fd, fmtr_bind->fd);
			fmtr_bind->flag = 0;
			return fmtr_bind->fd;
		}
	}
	if (ff_fdisused(fd))
	{
		return ff_bind(fd, (struct linux_sockaddr *)addr, addrlen);
	}
	else
	{
		return ft_real_func(bind)(fd, addr, addrlen);
	}
}

int ft_hook_accept(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
	printf("accept fd : %d \n",fd);
	int flag = 1;
	while (flag)
	{
		printf("0\n");
		if (fmt_form->flag == 0)
		{
			printf("1\n");
			fmt_form->flag = 1;
			fmt_form->form = ACCEPT;
			flag = 0;
		}
	}
	printf("2\n");
	fmt_accept->fd = fd;
	printf("3\n");
	struct sockaddr_in *cli = (struct sockaddr_in *)addr;
	printf("4\n");
	fmt_accept->address.sin_addr = cli->sin_addr;
	printf("5\n");
	fmt_accept->address.sin_family = cli->sin_family;
	printf("6\n");
	fmt_accept->address.sin_port = cli->sin_port;
	printf("7\n");
	int len = sizeof(fmt_accept->address);
	fmt_accept->addrlen_len = len;

	while (1)
	{
		if (fmtr_accept->flag == 1)
		{
			printf("accept ret:%d\n", fmtr_accept->fd);
			fmtr_accept->flag = 0;
			return fmtr_accept->fd;
		}
	}

	if (ff_fdisused(fd))
	{
		return ff_accept(fd, (struct linux_sockaddr *)addr, addrlen);
	}
	else
	{
		return ft_real_func(accept)(fd, addr, addrlen);
	}
}
