all:
	gcc -g server.c -lpthread -o server
	gcc -g client.c -o client
