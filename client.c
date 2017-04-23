#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h> //for signal disposition
#include <sys/socket.h>
#include <sys/types.h> //for WEXITSTATUS
#include <sys/wait.h> //for WEXITSTATUS
#include <unistd.h>
#include <pthread.h>
#include "common.h"
#include "utility.h"
#include "game_functions.h"

// typedef struct player_connection_data{
//     char name[20];
//     char ipaddr[20];
//     int port_no;
// } player_connection_data, *Player_connection_data;

player_connection_data my_data;
Player_connection_data conn_info;
int num_of_connected_players;
int my_socket;
char *network_data;

char server_ipaddress[40];
char my_ipaddress[40];
int server_tcp_port_no ;
int server_udp_port_no;
int my_tcp_port_no;
int my_udp_port_no;
int my_id;

void * pthread_work(void * data){
	char c;
	int arr[2];
	arr[0] = my_id;
	while (1){
		c = getchar();
		arr[1] = c;
		send_udp_wrapper(my_socket, arr, 2*sizeof(int), server_ipaddress, server_udp_port_no);
	}
}

int main(){
	printf("Enter server IP address - TCP PORT - UDP PORT\n");
	scanf("%s %d %d\n",server_ipaddress,&server_tcp_port_no,&server_udp_port_no);
	printf("Enter your IP address - TCP PORT - UDP PORT\n");
	scanf("%s %d %d\n",my_ipaddress,&my_tcp_port_no,&my_udp_port_no);
	
///////////////////////////////////////
	my_socket = socket(AF_INET, SOCK_STREAM, 0);
    int status = bind_wrapper(my_socket, my_ipaddress, my_tcp_port_no, 1);
    connect_wrapper(my_socket, server_ipaddress, server_tcp_port_no);
    strcpy(my_data.ipaddr,my_ipaddress); strcpy(my_data.name, "abhishek"); my_data.port_no = my_udp_port_no;
    send(my_socket, &my_data, sizeof(player_connection_data), 0);
    recv(my_socket, &num_of_connected_players, sizeof(int), 0);
    conn_info = (Player_connection_data)calloc(num_of_connected_players,sizeof(player_connection_data));
    recv(my_socket, conn_info, num_of_connected_players*sizeof(player_connection_data), 0);
    close(my_socket);

/////////////////////////////////////////
    my_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    network_data = (char*)calloc(num_of_connected_players,sizeof(char));
    initialize_game(num_of_connected_players);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, pthread_work, NULL);
    while (1){
    	sleep(0.1);
    	recvfrom(my_socket, network_data, num_of_connected_players*sizeof(char), 0, NULL, 0);
    	next_game_state(network_data);
    }
    pthread_join(thread_id, NULL);
    close(my_socket);
    return 0;
}