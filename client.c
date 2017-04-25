#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h> 
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h>
#include <pthread.h>
#include "common.h"
#include "utility.h"
#include "game_functions.h"
// #include "game_functions_datatypes.h"


#define MAX_PLAYERS 8

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
char name[40];
char **names;
int server_tcp_port_no ;
int server_udp_port_no;
int my_tcp_port_no;
int my_udp_port_no;
int my_id;
int status;
int rstatus;
struct gamestate;
extern struct gamestate gameinstance;
extern int NUM_OBSTACLES;
extern int NUM_FOOD_ITEMS;

void * sender_work(void * data){
    char c;
    while (1){
        c = getchar();
        if (send(my_socket, &c, sizeof(char), 0) != sizeof(char)){
            printf("problem in sending moves\nso quitting the game\n");
            exit(0);
        }
    }
}

int main(){
    printf("Enter server IP address - TCP PORT \n");
    strcpy(server_ipaddress, "192.168.43.32");
    server_tcp_port_no = 8005;
    // scanf("%s %d",server_ipaddress,&server_tcp_port_no);
    strcpy(my_ipaddress, "192.168.43.32");
    my_tcp_port_no = 8009;
    strcpy(name,"karan");
    // scanf("%s %d",my_ipaddress,&my_tcp_port_no);
    int i;
    my_socket = socket(AF_INET, SOCK_STREAM, 0);
    bind_wrapper(my_socket, my_ipaddress, my_tcp_port_no, 1);
    connect_wrapper(my_socket, server_ipaddress, server_tcp_port_no);
    printf("Connected to the server\n");
    int arr[2];
    size_t nr;
    if ((nr=send(my_socket,name,40*sizeof(char),0)) != 40*sizeof(char)){
    	perror("Error in sending name\n");
    	return 1;
    }
    names = (char**)malloc(MAX_PLAYERS*sizeof(char*));
    char * temp = (char*)malloc(sizeof(char)*40*MAX_PLAYERS);
    for (i=0;i<MAX_PLAYERS;i++)
    	names[i] = (char*)&temp[i*40];
    if (recv(my_socket, temp, 40*MAX_PLAYERS*sizeof(char), 0) != MAX_PLAYERS*sizeof(char)*40){
    	perror("Name not received correctly\n");
    }

    if (recv(my_socket, gameinstance.obstacles, NUM_OBSTACLES*sizeof(pair), 0) != NUM_OBSTACLES*sizeof(pair)){
        perror("Obstacles not received correctly\n");
    }

    if (recv(my_socket, gameinstance.food_items, NUM_FOOD_ITEMS*sizeof(pair), 0) != NUM_FOOD_ITEMS*sizeof(pair)){
        perror("Food Items not received correctly\n");
    }


    if ((nr = recv(my_socket, arr, 2*sizeof(int), 0)) != 2*sizeof(int)){
        printf("Bytes received : %lu\n", nr);
        perror("ERROR in receiving"); 
        return 1;
    }
    int num_of_connected_players = arr[0];
    for (i=0;i<num_of_connected_players;i++)
        printf("Received name : %s\n", names[i]);
    int my_id = arr[1];
    printf("Number of connected players: %d\n", num_of_connected_players);
    printf("Assigned ID : %d\n", my_id);


    network_data = (char*)calloc(num_of_connected_players,sizeof(char));
    initialize_game(num_of_connected_players, names);
    pthread_t sender_thread;
    pthread_create(&sender_thread, NULL, sender_work, NULL);
    while (1){
        if(recv(my_socket, network_data, num_of_connected_players*sizeof(char), 0) != sizeof(char)*num_of_connected_players){
            perror("Data received incorrectly!");
            printf("Server Error\n");
            exit(0);
        }
        next_game_state(network_data); //status = 1 means dead
    }
    pthread_join(sender_thread, NULL);
    close(my_socket);
    return 0;
}
