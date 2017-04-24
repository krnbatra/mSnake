#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <pthread.h>
#include "utility.h"
#include "common.h"

// typedef struct player_connection_data{
//     char name[20];
//     char ipaddr[20];
//     int port_no;
// } player_connection_data, *player_connection_data;



#define MAX_PLAYERS 8
#define FPS 20
const int waiting_time = 10;
int num_of_connected_players;
int num_of_alive_players;
int wait_min = 1;
player_connection_data conn_info[MAX_PLAYERS];
int alive[MAX_PLAYERS];
char network_data[MAX_PLAYERS];
int serverSocket;


void stop_listening(int signal){
    wait_min = 0;
}

void receive_moves(){
    int arr[2] = {};
    wait_min = 1;
    start_timer(0, 1000000/FPS);
    while (wait_min){
        if(recvfrom(serverSocket, arr, 2*sizeof(int), 0, NULL, 0) == -1);
        else {
            printf("Received move from %d = %d/%c\n", arr[0], arr[1], arr[1]);
            network_data[arr[0]] = arr[1];
        }
    }
}

void send_moves(){
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    for(int i = 0;i < num_of_connected_players; i++){
        if (alive[i] == 0) continue;
        client_addr.sin_addr.s_addr = inet_addr(conn_info[i].ipaddr);
        client_addr.sin_port = htons(conn_info[i].port_no);
        if(sendto(serverSocket, network_data,num_of_connected_players*sizeof(char), 0, (struct sockaddr*)&client_addr, sizeof(client_addr)) != num_of_connected_players*sizeof(char))
            perror("ERROR IN SENDING\n");
    }
}

void* work_pthread(void * dataptr){
    int *arr = (int*)dataptr;
    int newSocket = arr[0];
    int player_id = arr[1];
    size_t nr = recv(newSocket, &conn_info[player_id], sizeof(player_connection_data), 0);
    if (nr != sizeof(player_connection_data)) { printf("Data not received properly\n");  exit(2); }
    printf("Client connected --> ip = %s | port_no = %d | name = %s\n", 
                conn_info[player_id].ipaddr, conn_info[player_id].port_no, conn_info[player_id].name);
    while (wait_min) {
        sleep(1);
    }
    size_t ns = send(newSocket, &num_of_connected_players, sizeof(int), 0);
    ns = send(newSocket, conn_info, num_of_connected_players*sizeof(player_connection_data), 0);
    if (ns != sizeof(player_connection_data)*num_of_connected_players){
        perror("Failed to send player_information");
    }
    close(newSocket);
    pthread_exit(NULL);
}


int main(){

///////////////////////////////////
    printf("Enter server's IP Address : \n");
    char my_ip_address[40];
    scanf("%s",my_ip_address);
    int tcp_port_no;
    printf("Enter my TCP port no : \n");
    scanf("%d",&tcp_port_no);
    int udp_port_no;
    printf("Enter my UDP port no : \n");
    scanf("%d",&udp_port_no);

////////////////////////////////////
    signal_bind_wrapper(SIGALRM, stop_listening);
    printf("Listening for players\n");
    serverSocket = 0;
    int newSocket = 0;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int status = bind_wrapper(serverSocket, my_ip_address, tcp_port_no, 1);
    if (listen( serverSocket, 1)) {
        perror("Some problem in listen\n");
        exit(2);
    }
    pthread_t tid[MAX_PLAYERS];
    int i;
    start_timer(waiting_time, 0);
    for (i = 0; i < MAX_PLAYERS && wait_min; i++){
        newSocket = accept(serverSocket,NULL,NULL);
        if (newSocket != -1){
            int *arr = (int*)malloc(sizeof(int)*2);
            arr[0] = newSocket; arr[1] = num_of_connected_players++;
            pthread_create(&tid[i], NULL, work_pthread,arr);
        }
    }
    for (i = 0; i < num_of_connected_players; i++)
        pthread_join(tid[i], NULL);
    close(serverSocket);

//////////////////////////////////////
    printf("Starting game\n");
    printf("Number of players connected : %d\n", num_of_connected_players);
    num_of_alive_players = num_of_connected_players;
    for (i=0;i<num_of_alive_players;i++) alive[i] = 1;
    serverSocket = socket(AF_INET, SOCK_DGRAM,0);
    bind_wrapper(serverSocket, my_ip_address, udp_port_no,0);
    while (1){
        receive_moves();
        send_moves();
    }
    printf("Game over!\n");
    close(serverSocket);
    return 0;
}
