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

char html[] =
"HTTP/1.1 200 OK\r\n"
"Server: F-Stack\r\n"
"Date: Sat, 25 Feb 2017 09:26:33 GMT\r\n"
"Content-Type: text/html\r\n"
"Content-Length: 438\r\n"
"Last-Modified: Tue, 21 Feb 2017 09:44:03 GMT\r\n"
"Connection: keep-alive\r\n"
"Accept-Ranges: bytes\r\n"
"\r\n"
"<!DOCTYPE html>\r\n"
"<html>\r\n"
"<head>\r\n"
"<title>Welcome to F-Stack!</title>\r\n"
"<style>\r\n"
"    body {  \r\n"
"        width: 35em;\r\n"
"        margin: 0 auto; \r\n"
"        font-family: Tahoma, Verdana, Arial, sans-serif;\r\n"
"    }\r\n"
"</style>\r\n"
"</head>\r\n"
"<body>\r\n"
"<h1>Welcome to TEST_HOOK!</h1>\r\n"
"\r\n"
"<p>For online documentation and support please refer to\r\n"
"<a href=\"http://F-Stack.org/\">F-Stack.org</a>.<br/>\r\n"
"\r\n"
"<p><em>Thank you for using F-Stack.</em></p>\r\n"
"</body>\r\n"
"</html>";


int main(int argc,char * argv[])
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(80);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(sockfd,(struct sockaddr *)&my_addr,sizeof(my_addr));
    if(ret < 0)
    {
        printf("bind failed\n");
        exit(1);
    }

    ret = listen(sockfd,5);
    accept(sockfd,NULL,NULL);
    while(1)
    {
        char buf[256];
        size_t readlen = read(sockfd,buf,sizeof(buf));
        if(readlen>0)
        {
            write(sockfd,html,sizeof(html)-1);
        }
    }
}