#define MAX_SNAKE_LEN 50
#define MAX_PLAYERS 8
#define MAX_OBSTACLES 20
#define MAX_FOOD 20
#define MAX_BUFFER 25
#define MAX_PLAYER_NAME 50
#define MAXROW 50
#define MAXCOL 150

typedef int color_t;
typedef char* ipaddr_t;


typedef struct pair_t{
    int first, second;
} pair_t;

typedef enum direction_t{
    UP = 0, DOWN, LEFT, RIGHT
}direction_t;

typedef struct snake_t{
    struct pair_t points[MAX_SNAKE_LEN]; 
    int length;
    color_t color;
    direction_t dir;
}snake_t;

typedef struct player_t{
    int player_id;
    struct snake_t snake;
    ipaddr_t ippaddr;
    int port;
    int score;
    int highest_score;
    int rank;
    int num_of_wins;
    int alive;
    char name[MAX_PLAYER_NAME];
}player_t;

typedef struct move_t{
    int player_id;
    char key;
} move_t;

typedef struct gamedata_t{
    int height;
    int width;
    struct player_t players[MAX_PLAYERS];
    int level;
    struct pair_t food[MAX_FOOD];
    struct pair_t obstacles[MAX_OBSTACLES];
    char food_symbol;
    char obstacle_symbol;
    char request_buffer[MAX_BUFFER];
    int no_of_live_players;
    int no_of_initial_players;
    int no_of_obstacles;
    int no_of_food;
} gamedata_t;


typedef struct data_t{
    char ipaddr[20];
    int port_no;
    char name[20];
} data_t;


typedef struct players_info{
    int num_of_players;
    data_t player_info[MAX_PLAYERS];
    int assigned_id;
} players_info;

