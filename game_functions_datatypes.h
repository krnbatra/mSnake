#define MAX_SNAKE_LEN 50
#define MAX_PLAYERS 8
#define MAX_OBSTACLES 20
#define MAX_FOOD 20
#define MAX_BUFFER 25
#define MAX_PLAYER_NAME 50
#define HEIGHT 70
#define WIDTH 70

char up = 'w';
char left = 'a';
char down = 's';
char right = 'd';
char left_turn = 'j';
char right_turn = 'k';
typedef int color_t;

typedef enum direction_t{
    UP = 0, DOWN, LEFT, RIGHT
}direction_t;

typedef struct pair{
    int first;
    int second;
}pair, *Pair;

typedef struct snake{
    pair points[MAX_SNAKE_LEN]; 
    color_t length;
    int color;
    direction_t dir;
    int alive;
    int score;
}snake_t, *Snake;


typedef struct gamestate{
    Snake snake_list;
    pair food_items[MAX_FOOD];
    pair obstacles[MAX_OBSTACLES];
    int level;
    int num_of_snakes;
    int num_of_live_snakes;
    int num_of_food_items;
    int num_of_obstacles;
}gamestate, *Gamestate;

