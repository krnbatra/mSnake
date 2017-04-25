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
#include "game_functions_datatypes.h"

// typedef struct player_connection_data{
//     char name[20];
//     char ipaddr[20];
//     int port_no;
// } player_connection_data, *player_connection_data;

//Player with ID 0 is the controller of the server
#define FPS 10
const int waiting_time = 10;
int num_of_connected_players;
int num_of_alive_players;
int wait_min = 1;
player_connection_data conn_info[MAX_PLAYERS];
int alive[MAX_PLAYERS];
char network_data[MAX_PLAYERS];
int serverSocket;
int socket_data[MAX_PLAYERS];
char name[MAX_PLAYERS][40];
pair obstacles[NUM_OBSTACLES];
pair fooditems[NUM_FOOD_ITEMS];

void stop_listening(int signal){
    wait_min = 0;
}
int var;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void* work_sender(void *dataptr){
    int i;
    while (wait_min) {
        sleep(1);
    }
    sleep(1);
    while (1){
        wait_min = 1;
        start_timer(0,1000000/FPS);
        while (wait_min) ;
        var = 1;
        pthread_mutex_lock(&mutex1);
        for (i=0;i<num_of_connected_players;i++){
            if (!alive[i]) continue;
            send(socket_data[i], network_data, num_of_connected_players*sizeof(char), 0);
        }
        pthread_mutex_unlock(&mutex1);
        if (num_of_alive_players==0) break;
        var = 0;
    }
    pthread_exit(NULL);
}

void* client_handler(void * dataptr){
    int *arr = (int*)dataptr;
    int newSocket = arr[0];
    int player_id = arr[1];
    if (recv(newSocket, name[player_id], 40*sizeof(char), 0) != sizeof(char)*40){
    	perror("Name not received correctly\n");
    }
    printf("name received %s\n", name[player_id]);

    if (send(newSocket, obstacles, NUM_OBSTACLES*sizeof(pair), 0) != NUM_OBSTACLES*sizeof(pair)){
        perror("Obstacles not sent correctly\n");
    }
    else printf("Obstacles sent correctly to %d\n",player_id);
    if (send(newSocket, fooditems, NUM_FOOD_ITEMS*sizeof(pair), 0) != NUM_FOOD_ITEMS*sizeof(pair)){
        perror("Food items not sent correctly\n");
    }
    else printf("Food items sent correctly to %d\n",player_id);
    while (wait_min) {
        sleep(1);
    }
    arr[0] = num_of_connected_players;
    arr[1] = player_id;
    size_t ns;
    if (send(newSocket, name, 40*MAX_PLAYERS*sizeof(char), 0) != MAX_PLAYERS*sizeof(char)*40){
    	perror("Name not sent correctly\n");
    }
    if (send(newSocket, arr, 2*sizeof(int), 0) != 2*sizeof(int))
        perror("Failed in sending information\n");
    char move;
    while (1){
        //pthread_mutex_lock(&mutex1);
        if (recv(newSocket, &move, sizeof(char), 0) == sizeof(char)){    	
            printf("Received move from %d : %d/%c\n", player_id, move, move);
            if (move == 'X'){
                alive[player_id] = 0;
                num_of_alive_players--;
                printf("Player %d dead!\n", player_id);
                break;
            }
            pthread_mutex_lock(&mutex1);
            network_data[player_id] = move;
            pthread_mutex_unlock(&mutex1);
        }
        else{
            alive[player_id] = 0;
            num_of_alive_players--;
            printf("Player %d got disconnected\n", player_id);
            break;
        }
        //pthread_mutex_unlock(&mutex1);
    }
    close(newSocket);
    pthread_exit(NULL);
}


int main(){
    int i;
    int height = HEIGHT, width = WIDTH;
    for (i = 0; i < NUM_OBSTACLES; i++) {
        obstacles[i].first = rand()%(width-2)+2;
        obstacles[i].second = rand()%(height-2)+2;
        fooditems[i].first = rand()%(width-2)+2;
        fooditems[i].second = rand()%(height-2)+2;
    }
    printf("Enter server's IP Address : \n");
    char my_ip_address[40];
    //scanf("%s",my_ip_address);
    strcpy(my_ip_address, "172.17.49.75");
    int tcp_port_no;
    printf("Enter my TCP port no : \n");
    //scanf("%d",&tcp_port_no);
    tcp_port_no = 8005;
    serverSocket = 0;
    int newSocket = 0;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int status = bind_wrapper(serverSocket, my_ip_address, tcp_port_no, 1);
    if (listen( serverSocket, 3)) {
        perror("Some problem in listen\n");
        exit(2);
    }
    pthread_t tid[MAX_PLAYERS];
    wait_min = 1;
    signal_bind_wrapper(SIGALRM, stop_listening);
    printf("Listening for players\n");
    start_timer(waiting_time, 0);
    for (i = 0; i < MAX_PLAYERS && wait_min; i++){
        newSocket = accept(serverSocket,NULL,NULL);
        socket_data[num_of_connected_players] = newSocket;
        printf("Assigned socket %d to %d", newSocket, num_of_connected_players);
        alive[num_of_connected_players] = 1;
        if (newSocket != -1){
            int *arr = (int*)malloc(sizeof(int)*2);
            arr[0] = newSocket; arr[1] = num_of_connected_players++;
            pthread_create(&tid[i], NULL, client_handler,arr);
        }
    }
    printf("Number of connected players : %d\n", num_of_connected_players);
    num_of_alive_players = num_of_connected_players;
    pthread_t sender;
    pthread_create(&sender, NULL, work_sender, NULL);
    for (i = 0; i < num_of_connected_players; i++)
        pthread_join(tid[i], NULL);
    pthread_join(sender, NULL);
    printf("Game over!\n");
    close(serverSocket);
    return 0;
}
