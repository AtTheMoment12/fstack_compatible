#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 1234

#define MAXDATASIZE 100

int main(int argc, char *argv[])
{
    int sockfd, num;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in server, peer;
    if(argc != 3)
    {
        printf("Usage: %s <IP address> <message>\n", argv[0]);
        exit(1);
    }
    if((he = gethostbyname(argv[1]))==NULL)
    {
        printf("gethostbyname() error\n");
        exit(1);
    }
    if((sockfd=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket() error\n");
        exit(1);
    }
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = *( (struct in_addr *)he->h_addr);
    sendto(sockfd, argv[2], strlen(argv[2]), 0, (struct sockaddr *)&server, sizeof(server));
    socklen_t len;
    len = sizeof(server);
    while(1)
    {
        if((num = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr *)&peer, &len)) == -1)
        {
            printf("recvfrom() error\n");
            exit(1);
        }
/*
        if(len != sizeof(server) || memcmp((const void *)&server, (const void *)&peer, len) != 0 )
        {
            printf("Receive message from other server.\n");
            continue;
        }
		*/
        buf[num] = '\0';
        printf("Server Message: %s.\n", buf);
        break;
    }
    close(sockfd);
}
