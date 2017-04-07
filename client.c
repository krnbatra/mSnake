#include <stdlib.h>
#include <stdio.h>
#include "client_types.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

//next_game_state

struct gamedata_t;
struct pair_t;
struct player_t;
void move_snake(struct player_t * player, struct pair_t * food, int no_of_food);
void next_game_state(struct gamedata_t * gamestate);
void check_for_collision(struct gamedata_t * gamestate);
int establish_connection(char ip_addr[], int port_no);
int establish_connection(char ip_addr[], int port_no)// request TCP connection , user is assigned a unique id and prompt for the name .
{
	int clientSocket;
  char buffer[1024];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  printf("waiting for the connection\n");
     
  /*  Create the network socket.   */
      clientSocket = socket(PF_INET, SOCK_STREAM, 0);

       /*  1) PR_INET Internet domain
           2) SOCK_STREAM represents Stream socket
           3) 0 represents Default protocol (TCP in this case)
       */

  // Configure settings of the server address
  /* Address family = Internet */
     serverAddr.sin_family = AF_INET;

  /* Set port number */
     serverAddr.sin_port = htons(7891);
  /*  In networking we prefer to use big-endian binary 
      sequencing.  Since in our PC we use small endian binary
      scheme, we use htons to convert small endian to big
      endian.  */

  /* Set IP address to localhost */
     //char ip_addr[]="172.17.46.48";
  serverAddr.sin_addr.s_addr = inet_addr(ip_addr);

  /* Set all bits of the padding field to 0 */
     memset(serverAddr.sin_zero, '\0', sizeof
     serverAddr.sin_zero);

  /* Connect the socket to the server using the address*/
     addr_size = sizeof serverAddr;
     connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

  /* Read the message from the server into the buffer */
     recv(clientSocket, buffer, 1024, 0);

  /* Print the received message */
     printf("Data received:  %s\n",buffer);
  	 if (strcmp(buffer,"")==0)
  	 	return 0;
     char *userInput;
     userInput = (char *)malloc(sizeof(char)*10);

     printf("Enter your name: ");
     scanf("%[^\n\t]s", userInput);
     int inputLength;
     inputLength = strlen(userInput);
     if ( send ( clientSocket, userInput, inputLength, 0) != inputLength )
        {
        	printf("[ERROR] Unable to send the data.\n");
        	return 0;
        }
    
    	
    /* Read the message from the server into the buffer */
    
  return 1;
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