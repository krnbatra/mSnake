#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>  // library to close the connection
static int unique_id=0;
static char player_names[8][100];
#include <unistd.h>
#include <signal.h>
 

int main()
{
// Important declaration
//   int serverSocket = 0, newSocket = 0;
//   char buffer[1024];
  
//   struct sockaddr_in serverAddr;
//   struct sockaddr_storage serverStorage;
//   socklen_t addr_size;
//   /*  Create the network socket.   */
//   serverSocket = socket(PF_INET, SOCK_STREAM, 0);
//   // Configure settings of the server address
//   /* Address family = Internet */
//   serverAddr.sin_family = AF_INET;
//   /* Set port number */
//   serverAddr.sin_port = htons(7891);
//   /* Set IP address to localhost */
//     char ip_addr[]="172.17.46.179";
//   serverAddr.sin_addr.s_addr = inet_addr ( ip_addr );
//   /* Set all bits of the padding field to 0 */
//   memset ( serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero) );
  
//   memset(buffer, '0', sizeof(buffer));
// int yes=1;
// if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
//     perror("setsockopt");
//     exit(1);
// }
//   /*---- Bind the address struct to the socket ----*/
//   bind ( serverSocket, ( struct sockaddr *) &serverAddr, sizeof ( serverAddr ) );
//   /* Listen on the socket, with 5 max connection requests queued */
//   if ( listen ( serverSocket, 5 ) ==0 )
//       printf ( " Listening \n " );
//   else
//       printf("More than limit. Error!!  \n");
  
//   /* Accept call creates a new socket for the incoming connection */
//   char *recvBuffer;
//   recvBuffer = (char *)malloc(sizeof(char)*10);
    
    
//   while ( !listen ( serverSocket, 2 ) )
//   {
//       addr_size = sizeof ( serverStorage );
//       newSocket = accept ( serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
//       /* Send message to the socket of the incoming connection */

//       sprintf(buffer, "you were successfully registered.\nyour unique id is : %d\n", ++unique_id);

//       send  ( newSocket, buffer, 150, 0 );
//       int recvBytes;
//       if ( ( recvBytes = recv ( newSocket, recvBuffer, 9, 0) == -1))
//           printf("[ERROR] No input received from client.\n");
//       printf("%s entered the game\n",recvBuffer);
      
//       strcpy(player_names[unique_id-1],recvBuffer);
//       memset(recvBuffer,0,10);

//       close ( newSocket );
//       printf("current users %d\n",unique_id);
//       for (int i=0;i<unique_id;i++)
//       {
//         printf("%d.%s\n",i+1,player_names[i]);
//       }
//       printf("end users\n");
//       sleep ( 1 );
//   }
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
  char ip_addr[]="172.17.46.179";
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
  // char *recvBuffer;
  // recvBuffer = (char *)malloc(sizeof(char)*10);


  while ( !listen ( serverSocket, 2 ) )
  {
    addr_size = sizeof ( serverStorage );
    newSocket = accept ( serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
    /* Send message to the socket of the incoming connection */

    sprintf(buffer, "you were successfully registered.\nyour unique id is : %d\n", ++unique_id);

    send  ( newSocket, buffer, 150, 0 );
    // int recvBytes;
    // if ( ( recvBytes = recv ( newSocket, recvBuffer, 9, 0) == -1))
    //     printf("[ERROR] No input received from client.\n");
    // printf("%s entered the game\n",recvBuffer);
    
    // strcpy(player_names[unique_id-1],recvBuffer);
    // memset(recvBuffer,0,10);

    close ( newSocket );
    // code to display all the users at the server side start
    // printf("current users %d\n",unique_id);
    // for (int i=0;i<unique_id;i++)
    // {
    //   printf("%d.%s\n",i+1,player_names[i]);
    // }
    // printf("end users\n");
    // code to display all the users at the server side end
    sleep ( 1 );
  }
  // create a player object here and return that value
  return 0;
}
