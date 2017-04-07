all:
	gcc -g tcp_server.c -o server
	gcc -g tcp_client.c -o client
