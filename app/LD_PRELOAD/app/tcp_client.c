#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int main()
{
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
    assert(sockfd!=-1);
	struct sockaddr_in ser,cli;
    memset(&ser,0,sizeof(ser));
    ser.sin_family=AF_INET;
    ser.sin_port=htons(6000);
    ser.sin_addr.s_addr=inet_addr("10.1.1.3");
    printf("%s [%d:%d]\n", inet_ntoa(ser.sin_addr), ser.sin_addr.s_addr, ntohs(ser.sin_port));
    int res=connect(sockfd,(struct sockaddr*)&ser,sizeof(ser));
	assert(res!=-1);
	while(1)
    {
        printf("please input:");
        fflush(stdout);
        char buff[128]={0};
        fgets(buff,128,stdin);
        buff[strlen(buff)-1]=0;
        if(strcmp(buff,"end")==0)
        {
            break;
        }
        send(sockfd,buff,strlen(buff),0);
        char recvbuff[128]={0};
        recv(sockfd,recvbuff,127,0);
        printf("%s\n",recvbuff);
    }
    close(sockfd);
}
