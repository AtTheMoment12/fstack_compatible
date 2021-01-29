#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1234
#define MAXDATASIZE 100

int main(void)
{
    int sockfd;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;
    int num;
    char buf[MAXDATASIZE];
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Creating socket failed.\n");
        exit(1);
    }
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Bind() error.\n");
        exit(1);
    }
    
    len = sizeof(client);
    while(1)
    {
        num = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr *)&client, &len);
       if(num < 0)
        {
            perror("recvfrom() error.\n");
            exit(1);
        }
        buf[num] = '\0';
        printf("You got a message <%s> from client. \nIt's ip is %s, port is %d. \n", buf, inet_ntoa(client.sin_addr),htons(client.sin_port));
        sendto(sockfd, "Welcome\n", 8, 0, (struct sockaddr *)&client, len);
        if ( !strcmp(buf, "bye") ){
            break;
        }
    }
    close(sockfd);
}