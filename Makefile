all:
	gcc -g -c utility.c
	gcc -g server.c utility.o -lpthread -o server
