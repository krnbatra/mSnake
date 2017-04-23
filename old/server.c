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
#include "client_types.h"


int listening_port_no;
int server_port_no;

char * ip_address; // to be automated 
int next_free_id;
int wait_min = 1;
int over;
players_info info;


void change_over(int signo){
    over = 0;
}


void* work_pthread(void * dataptr){
    pair_t * pa = (pair_t *) dataptr;
    int newSocket = pa->first;
    int free_id = pa->second;
    size_t nr = recv(newSocket, &(info.player_info[free_id]), sizeof(data_t), 0);
    if (nr != sizeof(data_t)){
        printf("Data not received properly\n");
        exit(2);
    }
    printf("Client connected --> ip = %s | port_no = %d | name = %s\n", 
                info.player_info[free_id].ipaddr, info.player_info[free_id].port_no, info.player_info[free_id].name);
    while (wait_min) {
        sleep(1);
    }
    size_t ns = send(newSocket, &info, sizeof(players_info), 0);
    if (ns != sizeof(players_info)){
        perror("Failed to send player_information");
    }
    close(newSocket);
    pthread_exit(NULL);
}

//this function should run for 1 minute (timer not included yet) and will accept the connection requests and assign then unique ids starting from 1 and then ask for their name at the client side and saves the name of the client in an array corresponding to the id. 
void listen_for_players(){
    printf("Listening for players\n");
    int serverSocket = 0, newSocket = 0;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(listening_port_no);
    serverAddr.sin_addr.s_addr = inet_addr (ip_address);
    memset(serverAddr.sin_zero,'\0',sizeof(serverAddr.sin_zero));
    int yes=1;
    if (setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes))==-1){
        perror("setsockopt");
        exit(1);
    }
    if (bind(serverSocket,(struct sockaddr*) &serverAddr,sizeof(serverAddr)) == -1){
        perror("Problem in binding");
        exit(0);
    }
    if (listen( serverSocket, 1)) {
        printf("Some problem in listen\n");
        exit(2);
    }
    data_t player_data[MAX_PLAYERS];
    int cnt = MAX_PLAYERS;
    pthread_t tid[MAX_PLAYERS];
    int i;
    for (i = 0; i < MAX_PLAYERS && wait_min; i++){
        addr_size = sizeof(serverStorage);
        newSocket = accept(serverSocket,(struct sockaddr *)&serverStorage,&addr_size);
        if (newSocket != -1){
            pair_t *temp = (pair_t*)malloc(sizeof(pair_t));
            temp->first = newSocket;
            temp->second = next_free_id;
            pthread_create(&tid[i], NULL, work_pthread,temp);
            next_free_id++;
        }
    }
    for (i = 0; i < next_free_id; i++)
        pthread_join(tid[i], NULL);
    close(serverSocket);
}

int socket_no;
void send_game_moves(move_t* player_moves){
    struct sockaddr_in to_be_sent;
    to_be_sent.sin_family = AF_INET;
    for(int i = 0;i < next_free_id; i++){
        to_be_sent.sin_port = htons(info.player_info[i].port_no);
        to_be_sent.sin_addr.s_addr = inet_addr(info.player_info[i].ipaddr);
        if(sendto(socket_no, player_moves, sizeof(move_t)*MAX_PLAYERS, 0, (struct sockaddr*)&to_be_sent, sizeof(to_be_sent)) == -1){
            perror("ERROR IN SENDING\n");
        }else{
            printf("Sent game data to %c, %d\n", player_moves[i].key, player_moves[i].player_id);
        }
    }
}

// server receive
move_t* receive_moves(){
    move_t* returnMove = (move_t*)malloc(sizeof(move_t)*MAX_PLAYERS);
    memset(returnMove, '\0', sizeof(move_t)*MAX_PLAYERS);
    move_t single_data;
    over = 1;
    start_timer(0, 10000000);
    while (over){
        if(recvfrom(socket_no, &single_data, sizeof(move_t), 0, NULL, 0) == -1)
            perror("ERROR IN RECEIVING : ");
        else {
            
            printf("HELLO: %d %c\n", single_data.player_id, single_data.key);
            int id = single_data.player_id;
            returnMove[id] = single_data;
        }
    }
    return returnMove;
}

//periodic work
//receive moves
//send moves

void perioidic_work(){
   
}

int main(){
    custom_signal(SIGALRM, alarm_handler);
    start_timer(10, 0);
    ip_address = (char *) malloc(40*sizeof(char));
    strcpy(ip_address, "172.17.49.75");
    listening_port_no = 8054;
    server_port_no = 9001;
    next_free_id = 0;
    listen_for_players();
    socket_no = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in udp_bind;
    udp_bind.sin_family = AF_INET;
    udp_bind.sin_port = htons(server_port_no);
    udp_bind.sin_addr.s_addr = inet_addr(ip_address);
    if ( bind(socket_no, (struct sockaddr*) &udp_bind, sizeof(udp_bind)) == -1){
        perror("Failed to bind\n");
    }
    printf("HERERE\n");
    custom_signal(SIGALRM, change_over);
    perioidic_work();
    return 0;
}
