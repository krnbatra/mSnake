#include <stdio.h>
#include <stdlib.h> // for system command
#include <sys/types.h> //for WEXITSTATUS
#include <sys/wait.h> //for WEXITSTATUS
#include <signal.h> //for signal disposition
#include "conio.h"
#include "client_types.h"


//WEXITSTATUS returns the 8 bits of the the value returns by exit

#define HEIGHT 100
#define WIDTH 140


int draw_snake(struct snake_t *snake){
    int len = snake->length;
    int i;
    struct pair_t * arr = snake->points;
    textbackground(snake->color);
    for (i=0;i<len-1;i++){
        gotoxy(arr[i].first, arr[i].second);
        puts(" ");
    }
    gotoxy(arr[len-1].first,arr[len-1].second);
    textbackground(RED);
    puts(" ");
}

int draw_objects(struct pair_t * obj, int num){
    int i = 0;
    textbackground(CYAN);
    for (; i < num; i++){
        if (obj[i].first == -1) continue; //consumed
        gotoxy(obj[i].first, obj[i].second);
        puts("@");
    }
}

int draw_game_state(struct gamedata_t * game){
    clrscr();
    int width = game->width, height = game->height;
    int i = 0, j = 0;
    textbackground(LIGHTBLUE);
    textcolor(BLUE);
    for (j = 0; j < width; j++){
        gotoxy(0, j);
        puts("-");
        gotoxy(height-1,j);
        puts("-");
    }
    for (i = 0; i < height; i++){
        gotoxy(i, 0);
        puts("+");
        gotoxy(i,width-1);
        puts("+");
    }
    textbackground(LIGHTGREY);
    for (i = 1; i < height-1; i++){
        for (j = 1; j < width-1; j++){
            gotoxy(i,j);
            puts(" ");
        }
    }
    int num = game->no_of_initial_players;
    for (i = 0; i < num; i++){
        if (game->players[i].alive){
            draw_snake(&(game->players[i].snake));
        }
    }
    draw_objects(game->food, game->no_of_food);
    draw_objects(game->obstacles, game->no_of_obstacles);
    textattr(RESETATTR);
}

int main(){
    clrscr();
    char keypress;
    if (WEXITSTATUS(system("stty cbreak -echo stop u"))){
        fprintf(stderr, "Check if stty missing?\n");
        return 1;
    }
    struct snake_t snake;
    snake.length = 2;
    int i;
    for (i = 0; i < 2; i++){
        (snake.points)[i].second = i+2;
        snake.points[i].first = 10;
    }
    snake.color = 0x4;
    draw_snake(&snake);
    
    return 0;
}

