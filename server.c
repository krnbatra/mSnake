#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "client_types.h"


int listening_port_no;
char * ip_address; // to be automated 
int next_free_id;

struct player_t * listen_for_player();

struct player_t * listen_for_player(){ // this function should run for 1 minute (timer not included yet) and will accept the connection requests and assign then unique ids starting from 1 and then ask for their name at the client side and saves the name of the client in an array corresponding to the id. 
    int serverSocket = 0, newSocket = 0;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(listening_port_no);
    serverAddr.sin_addr.s_addr = inet_addr (ip_address);
    memset ( serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero) );
    int yes=1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    bind(serverSocket,(struct sockaddr*) &serverAddr,sizeof(serverAddr));
    if (listen( serverSocket, 1)) {
        printf("Some problem in listen\n");
        exit(2);
    }
    data_t player_data[MAX_PLAYERS];
    while ( !listen ( serverSocket, 1 ) ){
        addr_size = sizeof(serverStorage );
        newSocket = accept(serverSocket,(struct sockaddr *)&serverStorage,&addr_size);
        size_t nr = recv(newSocket, &(player_data[next_free_id]), sizeof(data_t), 0);
        if (nr != sizeof(data_t)){
            printf("Data not received properly\n");
            exit(2);
        }
        printf("Client connected\n ip = %s\nport_no = %d\nname = %s\n", 
                player_data[next_free_id].ipaddr, player_data[next_free_id].port_no, player_data[next_free_id].name);
        send(newSocket, &next_free_id, sizeof(int), 0 );
        next_free_id++;
        close (newSocket);
    }
}


int main(){
    listening_port_no = 8054;
    ip_address = (char *) malloc(40*sizeof(char));
    strcpy(ip_address, "172.17.49.75");
    next_free_id = 1;
    listen_for_player();

}
