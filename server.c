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
//#define FPS 10
int FPS = 10;
const int waiting_time = 20;
int num_of_connected_players;
int num_of_alive_players;
int wait_min = 1;
player_connection_data conn_info[MAX_PLAYERS];
int alive[MAX_PLAYERS];
char network_data[MAX_PLAYERS];
char buffer[MAX_PLAYERS];
int serverSocket;
int socket_data[MAX_PLAYERS];
char name[MAX_PLAYERS][40];
pair obstacles[NUM_OBSTACLES];
pair fooditems[NUM_FOOD_ITEMS];
int flag = 0;

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
        while(flag) {}
        wait_min = 1;
        start_timer(0,1000000/FPS);
        while (wait_min) ;
        pthread_mutex_lock(&mutex1);
        for (i=0;i<num_of_connected_players;i++)
            buffer[i] = network_data[i];
        pthread_mutex_unlock(&mutex1);
        int j;
        for (i=0;i<num_of_connected_players;i++){
            //printf("Sent to player %d over socket %d\n", i, socket_data[i]);
            if (!alive[i]) continue;
            send(socket_data[i], buffer, num_of_connected_players*sizeof(char), 0);
        }
        if (num_of_alive_players==0) break;
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
    else printf("Name received %s\n", name[player_id]);

    if (send(newSocket, obstacles, NUM_OBSTACLES*sizeof(pair), 0) != NUM_OBSTACLES*sizeof(pair)){
        perror("Obstacles not sent correctly\n");
    }
    else 
        printf("Obstacles sent correctly to %d\n",player_id);
    
    if (send(newSocket, fooditems, NUM_FOOD_ITEMS*sizeof(pair), 0) != NUM_FOOD_ITEMS*sizeof(pair)){
        perror("Food items not sent correctly\n");
    }
    else 
        printf("Food items sent correctly to %d\n",player_id);
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
        if (recv(newSocket, &move, sizeof(char), 0) == sizeof(char)){    	
            printf("Received move from %d : %d/%c\n", player_id, move, move);
            pthread_mutex_lock(&mutex1);
            network_data[player_id] = move;
            pthread_mutex_unlock(&mutex1);
            if (move == 'X'){
                alive[player_id] = 0;
                num_of_alive_players--;
                printf("Player %d quit!\n", player_id);
                break;
            }
            else if(move == 'P' && player_id == 0){
                flag = !flag;
            }
        }
        else {
            alive[player_id] = 1;
            printf("Game closed at player %d\n", player_id);
            network_data[player_id] = 'X';
            num_of_alive_players--;
            break;
        }
    }
    close(newSocket);
    pthread_exit(NULL);
}

void * level_handler(void * dataptr){
    int i = 0;
    while (1){
        sleep(10);
        FPS += FPS/5;
    }
}

int main(){
    int i;
    srand(time(NULL));
    int height = HEIGHT, width = WIDTH;
    for (i = 0; i < NUM_OBSTACLES; i++) {
        obstacles[i].first = rand()%(width-2)+2;
        obstacles[i].second = rand()%(height-2)+2;
        fooditems[i].first = rand()%(width-2)+2;
        fooditems[i].second = rand()%(height-2)+2;
    }
    printf("Enter server's IP Address : \n");
    char my_ip_address[40];
    scanf("%s",my_ip_address);
    int tcp_port_no;
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
    pthread_t sender,level;
    pthread_create(&sender, NULL, work_sender, NULL);
    pthread_create(&level, NULL, level_handler, NULL);
    for (i = 0; i < num_of_connected_players; i++)
        pthread_join(tid[i], NULL);
    pthread_join(sender, NULL);
    printf("Game over!\n");
    close(serverSocket);
    return 0;
}
