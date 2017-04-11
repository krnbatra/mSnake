#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h>

#define BUF_SIZE 2000
#define CLADDR_LEN 100

static int unique_id=0;
static int count = 0;
static char names[8][100];

int main(){
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
    while ( !listen ( serverSocket, 2 ) ){
        addr_size = sizeof(serverStorage );
        newSocket = accept(serverSocket,(struct sockaddr *)&serverStorage,&addr_size);
        send(newSocket, &next_free_id, sizeof(int), 0 );
        close (newSocket);
    }
}
