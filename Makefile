all:
	gcc -g -c utility.c
	gcc -g server.c utility.o -lpthread -o server
	gcc -g server1.c utility.o -lpthread -o server1
	gcc -g -c game_functions.c
	gcc -g client.c game_functions.o utility.o -lpthread -o client
