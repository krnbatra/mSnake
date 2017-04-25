#ifndef COMMON_H
#define COMMON_H
#define MAX_PLAYERS 8
#define HEIGHT 40
#define WIDTH 80
#define NUM_OBSTACLES 20
#define NUM_FOOD_ITEMS 20
typedef struct player_connection_data{
    char name[20];
    char ipaddr[20];
    int port_no;
} player_connection_data, *Player_connection_data;

#endif

