all:
	gcc -g -c utility.c
	gcc -g server.c utility.o -lpthread -o server
	gcc -g -c game_functions.c
	gcc -g client.c game_functions.o utility.o -lpthread -o client
