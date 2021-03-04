CFLAGS=-g
all:
	gcc $(CFLAGS) -o tcp_server tcp_server.c 
	gcc $(CFLAGS) -o tcp_client tcp_client.c
	gcc $(CFLAGS) -o udp_client udp_client.c
	gcc $(CFLAGS) -o udp_server udp_server.c
	gcc $(CFLAGS) -o http_server http_server.c
clean:
	rm -rf tcp_server tcp_client udp_client udp_server http_server
