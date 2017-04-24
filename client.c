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
int status;
int rstatus;

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
    strcpy(server_ipaddress, "172.17.49.75");
    server_tcp_port_no = 8005;
    server_udp_port_no = 9001;
  // scanf("%s %d %d",server_ipaddress,&server_tcp_port_no,&server_udp_port_no);
  printf("Enter your IP address - TCP PORT - UDP PORT\n");
  // scanf("%s %d %d",my_ipaddress,&my_tcp_port_no,&my_udp_port_no);
    strcpy(my_ipaddress, "172.17.49.75");
    my_tcp_port_no = 8006;
    my_udp_port_no = 9002;
  
///////////////////////////////////////
    my_socket = socket(AF_INET, SOCK_STREAM, 0);
    bind_wrapper(my_socket, my_ipaddress, my_tcp_port_no, 1);
    connect_wrapper(my_socket, server_ipaddress, server_tcp_port_no);
    strcpy(my_data.ipaddr,my_ipaddress); strcpy(my_data.name, "abhishek"); my_data.port_no = my_udp_port_no;
    size_t returnSend = send(my_socket, &my_data, sizeof(player_connection_data), 0);
    if(returnSend != sizeof(player_connection_data)){
        printf("ERROR in sending\n");
    }
    size_t returnRecv = recv(my_socket, &num_of_connected_players, sizeof(int), 0);
    if(returnRecv != sizeof(int)){
        printf("ERROR in receiving\n"); 
    }
    printf("Number of connected players: %d\n", num_of_connected_players);
    conn_info = (Player_connection_data)calloc(num_of_connected_players,sizeof(player_connection_data));
    size_t returnRecv2 = recv(my_socket, conn_info, num_of_connected_players*sizeof(player_connection_data), 0);
    if(returnRecv2 != num_of_connected_players*sizeof(player_connection_data))
        printf("ERROR in receving 2\n");
    for(int i = 0;i < num_of_connected_players; i++){
        if (!strcmp(conn_info[i].ipaddr, my_ipaddress) && conn_info[i].port_no==my_udp_port_no){
          my_id = i;
          break;
        }
    
    }
    close(my_socket);

/////////////////////////////////////////
    int i;
    my_socket = socket(AF_INET, SOCK_DGRAM, 0);
    bind_wrapper(my_socket, my_ipaddress, my_udp_port_no, 0);
    network_data = (char*)calloc(num_of_connected_players,sizeof(char));
    initialize_game(num_of_connected_players);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, pthread_work, NULL);
    while (1){
      if(recvfrom(my_socket, network_data, num_of_connected_players*sizeof(char), 0, NULL, NULL) != sizeof(char)*num_of_connected_players){
	perror("Data received incorrectly!");
      }
      next_game_state(network_data); //status = 1 means dead
    }
    pthread_join(thread_id, NULL);
    close(my_socket);
    return 0;
}
