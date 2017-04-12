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
char * ip_address; // to be automated 
int next_free_id;


int wait_min = 1;

players_info info;

void alarm_handler(int signal){
    if (signal == SIGALRM){
        wait_min = 0;
        printf("GAME STARTS IN 5 SEC\n");
    }
    else 
        fprintf(stderr, "Called by some by other interrupt");
}

void custom_signal(int signo, void (*func)(int)){
    struct sigaction * action = (struct sigaction*)malloc(sizeof(struct sigaction));
    action->sa_handler = func;
    sigemptyset(&(action->sa_mask));
    action->sa_flags = 0;
    sigaction(signo, action, NULL);
}

void start_timer(int sec){
    static struct itimerval tim;
    tim.it_value.tv_sec = sec;
    tim.it_value.tv_usec = 0;
    tim.it_interval.tv_sec = 0;
    tim.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &tim, NULL);
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
    printf("Client connected\n ip = %s\nport_no = %d\nname = %s\n", 
                info.player_info[free_id].ipaddr, info.player_info[free_id].port_no, info.player_info[free_id].name);
    while (wait_min) {
        printf("Hi I am %d, waiting... :(\n", free_id);
        sleep(2);
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


int main(){
    custom_signal(SIGALRM, alarm_handler);
    start_timer(10);
    ip_address = (char *) malloc(40*sizeof(char));
    strcpy(ip_address, "172.17.49.75");
    listening_port_no = 8054;
    next_free_id = 0;
    listen_for_players();
    return 0;
}
