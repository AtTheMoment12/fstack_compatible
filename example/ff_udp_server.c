#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <assert.h>

#include "ff_config.h"
#include "ff_api.h"
#include "ff_epoll.h"

#define PORT 1234
#define MAXDATASIZE 100
#define MAX_EVENTS 512

int sockfd;
struct sockaddr_in client;
socklen_t len;
char buf[MAXDATASIZE];
int a = 0,count = 0;
int loop(void *arg)
{
    int num = ff_recvfrom(sockfd,buf,MAXDATASIZE,0,(struct linux_sockaddr *)&client,&len);
    if(num < 0)
    {
        //printf("ff_recvfrom error\n");
        if(a>0)
        {
            printf("接收数据包%d个 平均%d个/60s\n",a,a/5);
            //count = count + a;
            //printf("共接收数据包%d个\n",count);
            a=0;
        }
    }else{
        a++;
        //buf[num] = '\0';
        //printf("You got a message <%s> from client. \nIt's ip is %s, port is %d. \n", buf, inet_ntoa(client.sin_addr),htons(client.sin_port));
        //ff_sendto(sockfd, "Welcome\n", 8, 0, (struct linux_sockaddr *)&client, len);
    }
}
int main(int argc, char *argv[])
{
    ff_init(argc, argv);
    sockfd = ff_socket(AF_INET, SOCK_DGRAM, 0);


    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = ff_bind(sockfd, (struct linux_sockaddr *)&my_addr, sizeof(my_addr));
    if (ret < 0)
    {
        printf("ff_bind failed\n");
        exit(1);
    }
    len = sizeof(client);
    
    ff_run(loop, NULL);
    return 0;
}