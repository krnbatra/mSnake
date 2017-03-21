#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ROWS 50
#define COLS 50
#define MAXN 50

// key bindings
char up = 'w';
char left = 'a';
char down = 's';
char right = 'd';

typedef struct pair{
	int x, y;
}pair;

typedef struct snake{
	int mDir;
	int mDead;
	int mLength;
	pair mLocation[MAXN];
	char mName[MAXN];
	int mSpeed;
	int mScore;
}snake;

typedef struct arena{
	pair mFood[MAXN];
	char mGrid[ROWS][COLS];
	int mLevel;
	int mNumplayers;
	snake mSnakes[MAXN];
}arena;

void initSnake(snake* sn, char name[]){
	sn->mDir = 0;
	sn->mDead = 0;
	sn->mLength = 2;
	// randomly place a snake at some location in the grid
	
	strcpy(sn->mName, name);
	sn->mSpeed = 0;
	sn->mScore = 0;
}

void placeFood(arena* ar, int numFood){
	for(int i = 0;i < numFood; i++){
		pair p;
		p.x = rand()%ROWS;
		p.y = rand()%COLS;
		ar->mFood[i] = p;
	}
}

void initArena(arena* ar, int numberOfSnakes, int level, char names[][3]){
	ar->mLevel = level;
	ar->mNumplayers = numberOfSnakes;
	// place food at random locations according to level
	if(level == 1){
		placeFood(ar, 4);
	}else if(level == 2){
		placeFood(ar, 8);
	}else if(level == 3){
		placeFood(ar, 12);
	}
	// initalize grid graphics 
	for(int i = 0;i < ROWS; i++){
		for(int j = 0;j < COLS; j++){
			ar->mGrid[i][j] = ' ';
		}
	}
	for(int i = 0;i < COLS; i++){
		ar->mGrid[0][i] = '-';
		ar->mGrid[ROWS-1][i] = '-';
	}

	for(int i = 0;i < ROWS; i++){
		ar->mGrid[i][0] = '|';
		ar->mGrid[i][COLS-1] = '|';
	}

	for(int i = 0;i < 4*level; i++){
		ar->mGrid[ar->mFood[i].x][ar->mFood[i].y] = '$';
	}

	initalize snakes
	for(int i = 0;i < numberOfSnakes; i++){
		initSnake(ar->mSnakes+ i, names[i]);
	}
	return;
}

void printArena(arena* ar){
	// print grid
	for(int i = 0;i < ROWS; i++){
		for(int j = 0;j < COLS; j++){
			printf("%c", ar->mGrid[i][j]);
		}
		printf("\n");
	}
	return;
}

void moveSnake(snake* sn, char key){
	return;
}

int main(){
	srand(time(NULL));
	arena ar;
	char names[1][3];
	strcpy(names[0], "kar");
	initArena(&ar, 1, 1, names);
	printArena(&ar);
	return 0;
}