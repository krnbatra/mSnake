#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h>

static int unique_id=0;

int main(){
    int serverSocket = 0, newSocket = 0;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
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
        addr_size = sizeof ( serverStorage );
        newSocket = accept ( serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
        sprintf(buffer, "%d", ++unique_id);
        send  ( newSocket, buffer, 150, 0 );
        close ( newSocket );
        sleep ( 1 );
    }
    return 0;
}
