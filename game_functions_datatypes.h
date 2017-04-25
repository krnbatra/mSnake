#ifndef GAME_FUNCTIONS_DATATYPES_H
#define GAME_FUNCTIONS_DATATYPES_H
#include "common.h"
#define MAX_SNAKE_LEN 50




typedef enum direction_t{
    UP = 0, DOWN, LEFT, RIGHT
}direction_t;

typedef struct pair{
    int first;
    int second;
}pair, *Pair;

typedef struct snake{
    pair points[MAX_SNAKE_LEN]; 
    int length;
    int color;
    direction_t dir;
    int alive;
    int score;
    char *name;
}snake_t, *Snake;


typedef struct gamestate{
    Snake snake_list;
    pair food_items[NUM_FOOD_ITEMS];
    pair obstacles[NUM_OBSTACLES];
    int level;
    int num_of_snakes;
    int num_of_live_snakes;
    int num_of_food_items;
    int num_of_obstacles;
}gamestate, *Gamestate;

#endif
