#include <stdlib.h>
#include <string.h>
#include "game_functions_datatypes.h"
#include "conio.h"


// typedef struct gamestate{
//     Snake snake_list;
//     pair food[MAX_FOOD];
//     pair obstacles[MAX_OBSTACLES];
//     int level;
//     int num_of_snakes;
// }gamestate, *Gamestate;


// typedef struct snake_t{
//     pair points[MAX_SNAKE_LEN]; 
//     color_t length;
//     int color;
//     direction_t dir;
//     int alive;
// }snake_t, *Snake;


gamestate gameinstance;

void draw_snake(Snake snake){
    int len = snake->length;
    int i;
    struct pair* arr = snake->points;
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

void draw_game_state(){
    clrscr();
    int width = WIDTH, height = HEIGHT;
    int i = 0, j = 0;
    gotoxy(1, 1);
    textcolor (BLUE);
    textbackground (CYAN);
    for (i = 0; i < width+2; i++){
        if (i == 0 || i == width+1)  printf ("+");
        else printf ("-");
    }
    textattr(RESETATTR);
    for (i = 0; i < height; i++){
        gotoxy (1, i + 2);
        textcolor (BLUE);
        textbackground (CYAN);
        printf ("|");
        textattr (RESETATTR);
        textcolor (WHITE);
        for (j = 0; j < width; j++)
           printf (" ");
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
    for (i = 0; i < num_of_snakes; i++)
        draw_snake(gameinstance.snake_list+i);
}

void initialize_game(int num_of_players){
    if (WEXITSTATUS(system("stty cbreak -echo"))){
        printf("Failed setting up the screen, is 'stty' missing?");
        exit(0);
    }
    
    int i,j;
    memset(&gameinstance, 0, sizeof(gamestate));
    num_of_snakes = num_of_players;
    gameinstance.snake_list = (Snake)calloc(num_of_players, sizeof(snake_t));
    for(i = 0; i < num_of_players; i++){
        gameinstance.snake_list[i].length = 10;
        gameinstance.snake_list[i].color = WHITE;
        gameinstance.snake_list[i].alive = 1;
        for(j = 0; j < gameinstance.snake_list[i].length ; j++){
            (gameinstance.snake_list[i].points)[j].first = j+5;
            (gameinstance.snake_list[i].points)[j].second = 5*i+5;
        }
    }
    draw_game_state(gameinstance);
}

void update_direction(Snake snake, char key){
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

void move_snake(Snake snake){
    if (snake->alive == 0) return;
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
    Pair arr = snake->points;
    textbackground (snake->color);
    gotoxy(arr[len-1].first, arr[len-1].second);
    printf(" ");
    textattr(RESETATTR);
    gotoxy(arr[len].first, arr[len].second);
    textbackground(RED);
    puts(" ");
    textattr(RESETATTR);
    int i = 0;
    gotoxy(arr[0].first,arr[0].second);
    puts(" ");
    // for (i  = 0;i < no_of_food; i++){
    //     if (food[i].first!=-1){
    //         if (new_headx == food[i].first && new_heady == food[i].second){
    //             len++;
    //             food[i].first = food[i].second = -1;
    //             player->score += 1;
    //             break;
    //         }
    //     }
    // }
    if (len>snake->length)
        snake->length = len;
    else {
        for (i = 0;i < len; i++)
            snake->points[i] = snake->points[i+1];
        (snake->points[len]).first = (snake->points[len].second) = -1;
    }
}


void next_game_state(char * moves){
    int i;
    for (i=0;i<num_of_snakes;i++)
        update_direction(gameinstance.snake_list+i, moves[i]);
    for (i=0;i<num_of_snakes;i++)
        if (gameinstance.snake_list[i].alive)
            move_snake(gameinstance.snake_list+i);
}





// void check_for_collision(gamestate * gamestate){
//     int n = gamestate->no_of_initial_players;
//     int i = 0, j = 0;
//     for (i = 0;i < n; i++){
//         struct player_t *player = &(gamestate->players[i]);
//         struct snake_t *snake = &(player->snake);
//         int len = snake->length;
//         int headx = (snake->points[len-1]).first, heady = (snake->points[len-1]).second;
//         if (!player->alive) continue;
//         // collision against walls
//         if (headx == 0 || headx == MAXROW-1 || heady == 0 || heady == MAXCOL-1){
//             player->alive = 0;
//             gamestate->no_of_live_players--;
//             continue;
//         }
//         // collision against obstacles
//         for (j = 0;j < gamestate->no_of_food; j++){
//             struct pair_t *obstacle = &(gamestate->obstacles[j]);
//             if (obstacle->first == headx && obstacle->second == heady){
//                 player->alive = 0;
//                 gamestate->no_of_live_players--;
//                 break;
//             }
//         }
//         if (!player->alive) continue;
//         // self collision
//         for (j = 0;j < (len-1); j++){
//             if (headx == (snake->points[j]).first && heady == (snake->points[j]).second ){
//                 player->alive = 0;
//                 gamestate->no_of_live_players--;
//                 break;
//             }
//         }
//         if (!player->alive) continue;
//         // collision against others
//         for (j = 0;j < n; j++){
//             if (j == i) continue;
//             struct player_t *player2 = &(gamestate->players[i]);
//             struct snake_t *snake2 = &(player2->snake);
//             int len2 = snake2->length;
//             if (!player->alive)     break;
//             if (!player2->alive)    continue;
//             int headx2 = (snake2->points[len2-1]).first, heady2 = (snake2->points[len2-1]).second;
//             if (headx == headx2 && heady == heady2){
//                 player->alive = 0;
//                 player2->alive = 0;
//                 gamestate->no_of_live_players--;
//                 continue;
//             }
//             int k = 0;
//             for(k = 0;k < (len2-1); k++){
//                 if (headx == (snake2->points[k]).first && heady == (snake2->points[k]).second){
//                     player->alive = 0;
//                     gamestate->no_of_live_players--;
//                     break;
//                 }
//             }
//         }
//     }
// }

// void draw_objects(pair_t * obj, int num){
//     int i = 0;
//     textbackground(CYAN);
//     for (; i < num; i++){
//         if (obj[i].first == -1) continue; //consumed
//         gotoxy(obj[i].first, obj[i].second);
//         puts("@");
//     }
// }


// void display_leaderboard(player_t * players, int no_of_players){
//     int i, j;
//     for(i = 0; i < no_of_players; i++){
//         for(j = 0; j < no_of_players-1; j++){
//             if(players[j].score < players[j+1].score){
//                 player_t temp = players[j];
//                 players[j] = players[j+1];
//                 players[j+1] = temp;
//             }
//         }
//     }
//     for(i = 0; i < no_of_players; i++){
//         printf("%d %s %d\n", i+1, players[i].name, players[i].score);
//     }

// }