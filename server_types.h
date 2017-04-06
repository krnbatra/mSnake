#define MAX_SNAKE_LEN 50
#define MAX_PLAYERS 8
#define MAX_OBSTACLES 20
#define MAX_FOOD 20
#define MAX_BUFFER 25

typedef int color_t;
typedef char* ipaddr_t;


struct pair_t{
    int first, second;
};

typedef enum direction_t{
    UP = 0, DOWN, LEFT, RIGHT
}direction_t;

struct snake_t{
    struct pair_t points[MAX_SNAKE_LEN]; 
    int length;
    color_t color;
    direction_t dir;
};

struct player_t{
    int player_id;
    struct snake_t snake;
    ipaddr_t ippaddr;
    int port;
    int score;
    int highest_score;
    int rank;
    int num_of_wins;
    int alive;
};

struct move_t{
    int player_id;
    char key;
};


struct gamedata_t{
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
};

