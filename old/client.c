#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h> //for signal disposition
#include <sys/socket.h>
#include <sys/types.h> //for WEXITSTATUS
#include <sys/wait.h> //for WEXITSTATUS
#include "conio.h"
#include <unistd.h>
#include "client_types.h"


void draw_snake(snake_t *snake);
void draw_objects(pair_t * obj, int num);
void draw_game_state(gamedata_t * game);
// int  initialize_gamestate(gamedata_t* game, player_t* players);
void display_leaderboard(player_t * players, int no_of_players);
players_info*  establish_connection(char server_ip_addr[], int port_no, data_t * data);
void send_move(move_t* player_move, int portno, char* server_ip_addr);
move_t* receive_moves(int portno, char* server_ip_addr);
void check_for_collision(gamedata_t * gamestate);
void next_game_state(gamedata_t * gamestate);
void move_snake(player_t * player, pair_t * food, int no_of_food);
void update_direction(player_t * player, char key);


char up = 'w';
char left = 'a';
char down = 's';
char right = 'd';
char left_turn = 'j';
char right_turn = 'k';
struct sockaddr_in udp_bind;


void draw_snake(snake_t *snake){
    int len = snake->length;
    int i;
    struct pair_t* arr = snake->points;
    textbackground (snake->color);
    for (i = 0; i < len-1; i++){
        gotoxy(arr[i].first, arr[i].second);
        printf(" ");
    }
    textattr(RESETATTR);
    gotoxy(arr[len-1].first, arr[len-1].second);
    textbackground(RED);
    puts(" ");
    textattr(RESETATTR);
}

void draw_objects(pair_t * obj, int num){
    int i = 0;
    textbackground(CYAN);
    for (; i < num; i++){
        if (obj[i].first == -1) continue; //consumed
        gotoxy(obj[i].first, obj[i].second);
        puts("@");
    }
}

void draw_game_state(gamedata_t * game){
    clrscr();
    int width = game->width, height = game->height;
    int i = 0, j = 0;
    gotoxy(1, 1);
    textcolor (BLUE);
    textbackground (CYAN);
	for (i = 0; i < width+2; i++){
		if (i == 0 || i == width+1)
			printf ("+");
		else
			printf ("-");
	}
    textattr(RESETATTR);
	for (i = 0; i < height; i++){

		gotoxy (1, i + 2);
		textcolor (BLUE);
		textbackground (CYAN);
		printf ("|");
		textattr (RESETATTR);

		textcolor (WHITE);
		for (j = 0; j < width; j++){
		   printf (" ");
		}

		textcolor (BLUE);
		textbackground (CYAN);
		printf ("|");
		textattr (RESETATTR);
	}
	gotoxy(1, height + 2);
	textcolor (BLUE);
    textbackground (CYAN);
	for (i = 0; i < width+2; i++){
		if (i == 0 || i == width+1)
			printf ("+");
		else
			printf ("-");
	}
	textattr(RESETATTR);

    int num = game->no_of_initial_players;
    for (i = 0; i < num; i++){
        if (game->players[i].alive){
            draw_snake(&(game->players[i].snake));
        }
    }
    // draw_objects(game->food, game->no_of_food);
    // draw_objects(game->obstacles, game->no_of_obstacles);
    // textattr(RESETATTR);
}

int initialize_gamestate(gamedata_t* gamedata, int num){
    int i, j;
    gamedata->no_of_initial_players = num;
    gamedata->height = MAXROW; 
    gamedata->width = MAXCOL;
    for(i = 0; i < num; i++){
        gamedata->players[i].snake.length = 10;
        gamedata->players[i].snake.color = WHITE;
        gamedata->players[i].alive = 1;
        for(j = 0; j < gamedata->players[i].snake.length ; j++){
            (gamedata->players[i].snake.points)[j].first = j+5;
            (gamedata->players[i].snake.points)[j].second = 5*i+5;
        }
    }
    draw_game_state(gamedata);
    return 0;
}

void display_leaderboard(player_t * players, int no_of_players){
    int i, j;
    for(i = 0; i < no_of_players; i++){
        for(j = 0; j < no_of_players-1; j++){
            if(players[j].score < players[j+1].score){
                player_t temp = players[j];
                players[j] = players[j+1];
                players[j+1] = temp;
            }
        }
    }
    for(i = 0; i < no_of_players; i++){
        printf("%d %s %d\n", i+1, players[i].name, players[i].score);
    }

}

players_info*  establish_connection(char server_ip_addr[], int port_no, data_t * data){
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_no);
    serverAddr.sin_addr.s_addr = inet_addr(server_ip_addr);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    /* Connect the socket to the server using the address*/
    addr_size = sizeof serverAddr;
    if (connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size) == -1){
        perror("Failed to connect to the server");
        exit(2);
    }
    /* Read the message from the server into the buffer */
    send(clientSocket, data, sizeof(data_t),0);
    players_info * info_req = (players_info*)malloc(sizeof(players_info));
    size_t nr = recv(clientSocket, info_req, sizeof(players_info), 0);
    if (nr == -1){
        perror("Error in receiving data from server");
        exit(2);
    }
    if (nr != sizeof(players_info)){
        printf("Problem in receving players information\n");
        exit(2);
    }
    else printf("Data received correctly!\n");
    int i;
    for (i = 0; i < info_req->num_of_players; i++){
        printf("%d --> %s %s %d\n", i, info_req->player_info[i].ipaddr, info_req->player_info[i].name, info_req->player_info[i].port_no);
    }
    return info_req;
}

int socket_no;
// client send move
void send_move(move_t* player_move, int portno, char* server_ip_addr){
    struct sockaddr_in to_be_sent;
    to_be_sent.sin_family = AF_INET;
    to_be_sent.sin_port = htons(portno);
    to_be_sent.sin_addr.s_addr = inet_addr(server_ip_addr);
    if(sendto(socket_no, player_move, sizeof(move_t), 0, (struct sockaddr*)&udp_bind, sizeof(udp_bind)) == 0){
        perror("ERROR IN SENDING\n");
    }


}

// client receive
move_t* receive_moves(int portno, char* server_ip_addr){
    move_t* returnMoves = (move_t*)malloc(sizeof(move_t)*MAX_PLAYERS);
    if(recvfrom(socket_no, returnMoves, sizeof(move_t)*MAX_PLAYERS, 0, NULL, 0) == -1){
        perror("ERROR IN RECEIVING");
    }
    return returnMoves;
}

void check_for_collision(struct gamedata_t * gamestate){
    int n = gamestate->no_of_initial_players;
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
                gamestate->no_of_live_players--;
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
    int n = gamestate->no_of_initial_players;
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

void update_direction(struct player_t * player, char key){
    struct snake_t *snake = &(player->snake);
    int previousDirection = snake->dir;
    if (key == right){
        snake->dir = RIGHT;
    }
    else if (key == left){
        snake->dir = LEFT;
    }
    else if (key == up){
        snake->dir = UP;
    }
    else if (key == down){
        snake->dir = DOWN;
    }
    else if (key == left_turn){
        switch (previousDirection){
            case LEFT:
                snake->dir = DOWN;
                break;

            case RIGHT:
                snake->dir = UP;
                break;

            case UP:
                snake->dir = LEFT;
                break;

            case DOWN:
                snake->dir = RIGHT;
                break;
        }
    }
    else if (key == right_turn){
        switch (previousDirection){
            case LEFT:
                snake->dir = UP;
                break;

            case RIGHT:
                snake->dir = DOWN;
                break;

            case UP:
                snake->dir = RIGHT;
                break;

            case DOWN:
                snake->dir = LEFT;
                break;
        }
    }
}

int fetch_id(players_info* players, char* ip_addr){
    for(int i = 0;i < players->num_of_players; i++){
        if(strcmp(players->player_info[i].ipaddr, ip_addr) == 0){
            return i;
        }
    }
    return -1;
}

void periodic_work(gamedata_t * gamedata, int id,players_info* player_info_pointer){
	while (1){
		sleep(0.1);
		draw_game_state(gamedata);
		char c = getchar();
		
		move_t move;
		move.player_id = id;
		move.key = c;
		if(sendto(socket_no, &move, sizeof(move_t), 0, (struct sockaddr*)&udp_bind, sizeof(udp_bind)) == -1){
	        perror("ERROR IN SENDING\n");
	    }
	    move_t *array = receive_moves(9001, "127.0.0.1");
	    int i;
	    for (i=0;i<gamedata->no_of_initial_players;i++){
	    	if (array[i].key != 'x')
	    		update_direction(&(gamedata->players[i]), array[i].key);
	    }
	    next_game_state(gamedata);
	}
}

int main(){
	if (WEXITSTATUS(system("stty cbreak -echo stop u"))){
        fprintf(stderr, "Check if stty missing?\n");
        return 1;
    }
    data_t var;
    strcpy(var.ipaddr, "127.0.0.1");
    var.port_no = 1009;
    strcpy(var.name, "modi");
    players_info* player_info_pointer = establish_connection("127.0.0.1", 8054, &var);
    socket_no = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    udp_bind.sin_family = AF_INET;
    udp_bind.sin_port = htons(9002);
    udp_bind.sin_addr.s_addr = inet_addr("127.0.0.1");
    if ( bind(socket_no, (struct sockaddr*) &udp_bind, sizeof(udp_bind)) == -1){
        perror("Failed to bind\n");
    }
    gamedata_t* gamedata = (gamedata_t *)malloc(sizeof(gamedata_t));
    int id = fetch_id(player_info_pointer, "127.0.0.1");
    printf("Number of Players: %d", player_info_pointer->num_of_players);
    initialize_gamestate(gamedata, player_info_pointer->num_of_players);
    periodic_work(gamedata, id,player_info_pointer);

    clrscr ();

   	return WEXITSTATUS(system ("stty sane"));
}
