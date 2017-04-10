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
	struct sockaddr_in ips[3];
    int serverSocket = 0, newSocket = 0;
    char buffer[BUF_SIZE];
    struct sockaddr_in serverAddr, cl_addr;
    // struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    char clientAddr[CLADDR_LEN];
    /*  Create the network socket.   */
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    // Configure settings of the server address
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number */
    serverAddr.sin_port = htons(7891);
    /* Set IP address to localhost */
    char ip_addr[] = "172.17.46.179";
    serverAddr.sin_addr.s_addr = inet_addr ( ip_addr );
    /* Set all bits of the padding field to 0 */
    memset ( serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero) );

    memset(buffer, '0', sizeof(buffer));
    int yes=1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    /*---- Bind the address struct to the socket ----*/
    bind ( serverSocket, ( struct sockaddr *) &serverAddr, sizeof ( serverAddr ) );
    /* Listen on the socket, with 5 max connection requests queued */
    if ( listen ( serverSocket, 5 ) ==0 )
        printf ( " Listening \n " );
    else
        printf("More than limit. Error!!  \n");

    /* Accept call creates a new socket for the incoming connection */

    while ( !listen ( serverSocket, 2 ) ){
        addr_size = sizeof ( cl_addr );
        newSocket = accept ( serverSocket, (struct sockaddr *) &cl_addr, &addr_size);
        inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, CLADDR_LEN);
        int ret = recvfrom(newSocket, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, &addr_size);
        if(ret < 0) {
            printf("Error receiving data!\n");  
            exit(1);
        }
        strcpy(names[count], buffer);
        ips[count] = cl_addr;
        count++;
        printf("Received data from %s: %s\n", clientAddr, buffer); 
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d", ++unique_id);
        // ret = sendto( newSocket, buffer, 150, 0, (struct sockaddr *) &cl_addr, addr_size);
        if(count == 2){
            for(int i = 0;i < 2; i++){
                for(int j = 0;j < 2; j++){
                    ret = sendto(newSocket, names[j], BUF_SIZE, 0, (struct sockaddr *) &ips[i], addr_size);
                    printf("%s\n", names[j]);
                    memset(buffer, 0, sizeof(buffer));
                }
            }
        }
        close ( newSocket );
        sleep ( 1 );
    }
    return 0;
}
