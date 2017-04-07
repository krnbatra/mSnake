#include <stdlib.h>
#include <stdio.h>
#include "server_types.h"
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

//next_game_state

struct gamedata_t;
struct pair_t;
struct player_t;
void move_snake(struct player_t * player, struct pair_t * food, int no_of_food);
void next_game_state(struct gamedata_t * gamestate);
void check_for_collision(struct gamedata_t * gamestate);
struct player_t * listen_for_player(int next_free_id);

struct player_t * listen_for_player() // this function should run for 1 minute (timer not included yet) and will accept the connection requests and assign then unique ids starting from 1 and then ask for their name at the client side and saves the name of the client in an array corresponding to the id. 
{
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
    char ip_addr[]="172.17.46.48";
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
  char *recvBuffer;
  recvBuffer = (char *)malloc(sizeof(char)*10);
    
    
  while ( !listen ( serverSocket, 2 ) )
  {
      addr_size = sizeof ( serverStorage );
      newSocket = accept ( serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
      /* Send message to the socket of the incoming connection */

      sprintf(buffer, "you were successfully registered.\nyour unique id is : %d\n", ++unique_id);

      send  ( newSocket, buffer, 150, 0 );
      int recvBytes;
      if ( ( recvBytes = recv ( newSocket, recvBuffer, 9, 0) == -1))
          printf("[ERROR] No input received from client.\n");
      printf("%s entered the game\n",recvBuffer);
      
      strcpy(player_names[unique_id-1],recvBuffer);
      memset(recvBuffer,0,10);

      close ( newSocket );
      // code to display all the users at the server side start
      printf("current users %d\n",unique_id);
      for (int i=0;i<unique_id;i++)
      {
        printf("%d.%s\n",i+1,player_names[i]);
      }
      printf("end users\n");
      // code to display all the users at the server side end
      sleep ( 1 );
  }
  // create a player object here and return that value
  

}

void check_for_collision(struct gamedata_t * gamestate){
	int n = gamestate->no_of_live_players;
	int i = 0, j = 0;
	for (i = 0;i < n; i++){
		struct player_t *player = &(gamestate->players[i]);
		struct snake_t *snake = &(player->snake);
		int len = snake->length;
		int headx = (snake->points[len-1]).first, heady = (snake->points[len-1]).second;
		if (!player->alive)	continue;
		// collision against walls
		if (headx == 0 || headx == MAXROW-1 || heady == 0 || heady == MAXCOL-1){
			player->alive = 0;
			gamestate->no_of_live_players--;
			continue;
		}
		// collision against obstacles
		for (j = 0;j < gamestate->no_of_food; j++){
			struct pair_t *obstacle = &(gamestate->obstacles[j]);
			if (obstacle->first == headx && obstacle->second == heady){
				player->alive = 0;
				gamestate->no_of_live_players--;
				break;
			}
		}
		if (!player->alive)	continue;
		// self collision
		for (j = 0;j < (len-1); j++){

			if (headx == (snake->points[j]).first && heady == (snake->points[j]).second ){
				player->alive = 0;
				gamestate->no_of_live_players--;
				break;
			}
		}
		if (!player->alive)	continue;
		// collision against others
		for (j = 0;j < n; j++){
			if (j == i)	continue;
			struct player_t *player2 = &(gamestate->players[i]);
			struct snake_t *snake2 = &(player2->snake);
			int len2 = snake2->length;
			if (!player->alive)		break;
			if (!player2->alive)	continue;
			int headx2 = (snake2->points[len2-1]).first, heady2 = (snake2->points[len2-1]).second;
			if (headx == headx2 && heady == heady2){
				player->alive = 0;
				player2->alive = 0;
				gamestate->no_of_live_players -= 2;
				continue;
			}
			int k = 0;
			for(k = 0;k < (len2-1); k++){
				if (headx == (snake2->points[k]).first && heady == (snake2->points[k]).second){
					player->alive = 0;
					gamestate->no_of_live_players--;
					break;
				}
			}
		}
	}

}

void next_game_state(struct gamedata_t * gamestate){
    int n = gamestate->no_of_live_players;
    for (int i = 0;i < n; i++)
        move_snake(&(gamestate->players[i]), gamestate->food, gamestate->no_of_food);
    check_for_collision(gamestate);
}

void move_snake(struct player_t * player, struct pair_t * food, int no_of_food){
    if (player->alive == 0) return;
    struct snake_t *snake = &(player->snake);
    int len = snake->length;
    int old_headx = (snake->points[len-1]).first, old_heady = (snake->points[len-1]).second;
    int new_headx = -1, new_heady = -1;
    switch(snake->dir){
        case UP : 
            new_headx = old_headx; new_heady = old_heady-1;
            break;
        case DOWN :
            new_headx = old_headx; new_heady = old_heady+1;
            break;
        case LEFT :
            new_headx = old_headx-1; new_heady = old_heady;
            break;
        case RIGHT :
            new_headx = old_headx+1; new_heady = old_heady;
            break;
        default :
            printf("Invalid snake direction\n");
            exit(1);
    }
    (snake->points[len]).first = new_headx;
    (snake->points[len]).second = new_heady;
    int i = 0;

    for (i  = 0;i < no_of_food; i++){
        if (food[i].first!=-1){
            if (new_headx == food[i].first && new_heady == food[i].second){
                len++;
                food[i].first = food[i].second = -1;
                player->score += 1;
                break;
            }
        }
    }
    if (len>snake->length)
        snake->length = len;
    else {
        for (i = 0;i < len; i++)
            snake->points[i] = snake->points[i+1];
        (snake->points[len]).first = (snake->points[len].second) = -1;
    }
    return;
}