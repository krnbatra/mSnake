#include <stdlib.h>
#include <stdio.h>
#include "server_types.h"


//next_game_state

struct gamedata_t;
struct pair_t;
struct player_t;
void move_snake(struct player_t * player, struct pair_t * food, int no_of_food);
void check_for_collision(struct gamedata_t * gamestate);



void next_game_state(struct gamedata_t * gamestate){
    int n = gamestate->no_of_initial_players;
    for (int i=0;i<n;i++)
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

    for (i=0;i<no_of_food;i++){
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
        for (i=0;i<len;i--)
            snake->points[i] = snake->points[i+1];
    }
    return;
}
