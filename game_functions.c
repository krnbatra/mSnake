#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game_functions_datatypes.h"
#include "conio.h"
#include "common.h"


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
char up = 'w';
char left = 'a';
char down = 's';
char right = 'd';
char left_turn = 'j';
char right_turn = 'k';
typedef int color_t;

void initialize_game(int num_of_players, char ** name);
void draw_game_state();
void check_for_collision();
void draw_snake(Snake snake);
void draw_objects();
void update_direction(Snake snake, char key);
void move_snake(Snake snake);
int next_game_state(char * moves);
void display_leaderboard();

extern int my_id;

gamestate gameinstance;

void initialize_game(int num_of_players, char **names){
    if (WEXITSTATUS(system("stty cbreak -echo"))){
        printf("Failed setting up the screen, is 'stty' missing?");
        exit(0);
    }
    int i,j;
    gameinstance.num_of_snakes = num_of_players;
    gameinstance.num_of_live_snakes = num_of_players;
    gameinstance.snake_list = (Snake)calloc(num_of_players, sizeof(snake_t));
    for(i = 0; i < num_of_players; i++){
        gameinstance.snake_list[i].length = 10;
        gameinstance.snake_list[i].color = WHITE;
        gameinstance.snake_list[i].alive = 1;
        gameinstance.snake_list[i].dir = RIGHT;
        printf("Name here is %s\n", names[i]);
        gameinstance.snake_list[i].name = names[i];
        printf("Name copied : %s\n", gameinstance.snake_list[i].name);
        for(j = 0; j < gameinstance.snake_list[i].length ; j++){
            (gameinstance.snake_list[i].points)[j].first = j+5;
            (gameinstance.snake_list[i].points)[j].second = 5*i+5;
        }
    }
    gameinstance.num_of_obstacles = NUM_OBSTACLES;
    gameinstance.num_of_food_items = NUM_FOOD_ITEMS;
    printf("%d %d\n", gameinstance.num_of_obstacles, gameinstance.num_of_food_items);
    draw_game_state(gameinstance);
}

void draw_game_state(){
    clrscr();
    int width = WIDTH, height = HEIGHT;
    int i = 0, j = 0;
    gotoxy(1, 1);
    textcolor (BLUE);
    textbackground (CYAN);
    for (i = 1; i <=width; i++){
        printf("-");
    }
    textattr(RESETATTR);
    for (i = 2; i < height; i++){
        gotoxy (1,i);
        textcolor (BLUE);
        textbackground (CYAN);
        printf ("|");
        textattr (RESETATTR);
        textcolor (WHITE);
        for (j = 2; j < width; j++)
           printf (" ");
        textcolor (BLUE);
        textbackground (CYAN);
        printf ("|");
        textattr (RESETATTR);
    }
    gotoxy(1, height);
    textcolor (BLUE);
    textbackground (CYAN);
    for (i = 1; i <= width; i++){
        printf ("-");
    }
    textattr(RESETATTR);
    for (i = 0; i < gameinstance.num_of_snakes; i++)
        draw_snake(gameinstance.snake_list+i);
    draw_objects();
}

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

void draw_objects(){
    srand(time(NULL));
    int i;

    textcolor(RED);
    textbackground(RED);
    for (i = 0; i < gameinstance.num_of_obstacles; i++){
        if (gameinstance.obstacles[i].first == -1) continue; //consumed
        gotoxy(gameinstance.obstacles[i].first, gameinstance.obstacles[i].second);
        printf("$");
    }
    textcolor(YELLOW);
    textbackground(YELLOW);
    for (i = 0; i < gameinstance.num_of_food_items; i++) {
        if (gameinstance.food_items[i].first == -1) continue; //consumed
        gotoxy(gameinstance.food_items[i].first, gameinstance.food_items[i].second);
        printf("@");
    }
    textattr(RESETATTR);
}

void update_direction(Snake snake, char key){
    int previousDirection = snake->dir;
    if (key == right && previousDirection!=LEFT){
        snake->dir = RIGHT;
    }
    else if (key == left && previousDirection!=RIGHT){
        snake->dir = LEFT;
    }
    else if (key == up && previousDirection!=DOWN){
        snake->dir = UP;
    }
    else if (key == down && previousDirection!=UP){
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
    for (i  = 0;i < gameinstance.num_of_food_items; i++){
        if (gameinstance.food_items[i].first!=-1){
            if (new_headx == gameinstance.food_items[i].first && new_heady == gameinstance.food_items[i].second){
                len++;
                gameinstance.food_items[i].first = gameinstance.food_items[i].second = -1;
                snake->score += 1;
                break;
            }
        }
    }
    if (len > snake->length)
        snake->length = len;
    else {
        for (i = 0;i < len; i++)
            snake->points[i] = snake->points[i+1];
        (snake->points[len]).first = (snake->points[len].second) = -1;
    }
}

int next_game_state(char * moves){
    int i;
    for (i=0;i<gameinstance.num_of_snakes;i++)
        update_direction(gameinstance.snake_list+i, moves[i]);
    for (i=0;i<gameinstance.num_of_snakes;i++)
        if (gameinstance.snake_list[i].alive)
            move_snake(gameinstance.snake_list+i);
    check_for_collision();
    display_leaderboard();
    return !gameinstance.snake_list[my_id].alive;
}

void check_for_collision(){
    int n = gameinstance.num_of_snakes;
    int i = 0, j = 0;
    int status[n];
    memset(status, 0, n*sizeof(int));
    for (i = 0;i < n; i++){
        Snake snake = gameinstance.snake_list+i;
        int len = snake->length;
        int headx = (snake->points[len-1]).first, heady = (snake->points[len-1]).second;
        if (!snake->alive) continue;
        // collision against walls
        if (headx == 1 || headx == WIDTH || heady == 1 || heady == HEIGHT){
            status[i] = 1;
            continue;
        }
        // collision against obstacles
        for (j = 0;j < gameinstance.num_of_obstacles; j++){
            if (gameinstance.obstacles[j].first == headx && gameinstance.obstacles[j].second == heady){
                status[i] = 1;
                break;
            }
        }
        if (status[i]) continue;
        // self collision
        for (j = 0;j < (len-1); j++){
            if (headx == snake->points[j].first && heady == snake->points[j].second ){
                status[i] = 1;
                break;
            }
        }
        if (status[i]) continue;
        // collision against others
        for (j = 0;j < n; j++){
            if (j == i) continue;
            Snake snake2 = gameinstance.snake_list + j;
            if (!snake2->alive) continue;
            int len2 = snake2->length;
            int headx2 = (snake2->points[len2-1]).first, heady2 = (snake2->points[len2-1]).second;
            int k = 0;
            for(k = 0;k < len2; k++){
                if (headx == (snake2->points[k]).first && heady == (snake2->points[k]).second){
                    status[i] = 1;
                    break;
                }
            }
        }
    }
    for (i=0;i<n;i++){
    	if (status[i]){
            //remove_snake
            int j;
            Snake snake = gameinstance.snake_list + i;
            textattr(RESETATTR);
            for (j = 0; j < snake->length; j++){
                gotoxy(snake->points[j].first,snake->points[j].second);
                printf(" ");
            }
            gameinstance.snake_list[i].alive = 0;
            gameinstance.num_of_live_snakes--;
            if (gameinstance.num_of_live_snakes == 0){
                clrscr();
                system("stty sane");
                printf("Game over!\n");
                exit(0);
            }
    	}
    }
}

void display_leaderboard(){
    int i, j;
    char **names = (char**)calloc(gameinstance.num_of_snakes,sizeof(char*));
    int *score = (int*)calloc(gameinstance.num_of_snakes,sizeof(int));
    for (i=0;i<gameinstance.num_of_snakes;i++)
        names[i] = gameinstance.snake_list[i].name;
    for (i=0;i<gameinstance.num_of_snakes;i++)
        score[i] = gameinstance.snake_list[i].score;
    char * temp;
    int stemp;
    for (i=1;i<gameinstance.num_of_snakes;i++){
       j = i-1;
       stemp = score[i];
       temp = names[i];
       while (j>=0 && score[j]<stemp){
           names[j+1] = names[j];
           score[j+1] = score[j];
           j--;
       }
       score[j+1] = stemp;
       names[j+1] = temp;
    }
    textcolor(RED);
    textbackground(YELLOW);
    gotoxy(WIDTH+1, 1);
    printf("%-4s | %-10s | %-5s\n", "RANK", "NAME", "SCORE");
    for(i = 0; i < gameinstance.num_of_snakes; i++){
        gotoxy(WIDTH+1, i+2);
        textcolor(RED);
        textbackground(YELLOW);
        printf("%-4d | %-10s | %-5d\n", i+1,names[i], score[i]);
    }
    textattr(RESETATTR);
}
