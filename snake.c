#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ROWS 50
#define COLS 120
#define MAXN 50

// key bindings
char up = 'w';
char left = 'a';
char down = 's';
char right = 'd';

typedef enum {UP = 0, LEFT, DOWN, RIGHT} directions;

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
	sn->mDead = 0;
	sn->mLength = 2;
	// randomly place a snake at some location in the grid
	int startX = rand() % ROWS;
	int startY = rand() % COLS;
	sn->mDir = rand() % 4;
	for(int i = 0;i < sn->mLength; i++){
		pair p;
		if(sn->mDir == LEFT){
			p.x = startX;
			p.y = startY + i;
		}else if(sn->mDir == UP){
			p.x = startX + i;
			p.y = startY;
		}else if(sn->mDir == DOWN){
			p.x = startX - i;
			p.y = startY;
		}else if(sn->mDir == RIGHT){
			p.x = startX;
			p.y = startY - i;
		}
		sn->mLocation[i] = p;
	}
	strcpy(sn->mName, name);
	sn->mSpeed = 0;
	sn->mScore = 0;
}

void placeFood(arena* ar, int numFood){
	for(int i = 0;i < numFood; i++){
		pair p;
		do{
			p.x = rand()%ROWS;
			p.y = rand()%COLS;
		}while(ar->mGrid[p.x][p.y] != ' ');
		ar->mFood[i] = p;
	}
}

void initArena(arena* ar, int numberOfPlayers, int level, char names[][MAXN]){
	srand(time(NULL));
	ar->mLevel = level;
	ar->mNumplayers = numberOfPlayers;
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
	
	placeFood(ar, 4*level);
	
	for(int i = 0;i < 4*level; i++){
		ar->mGrid[ar->mFood[i].x][ar->mFood[i].y] = '$';
	}

	//initalize snakes
	for(int i = 0;i < numberOfPlayers; i++){
		initSnake(ar->mSnakes+ i, names[i]);
	}

	for(int i = 0;i < numberOfPlayers; i++){
		for(int j = 0;j < ar->mSnakes[i].mLength; j++){
			ar->mGrid[ar->mSnakes[i].mLocation[j].x][ar->mSnakes[i].mLocation[j].y] = '#';
		}
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
	arena ar;
	int numberOfPlayers;
	printf("Enter number of players\n");
	scanf("%d", &numberOfPlayers);
	char playerNames[numberOfPlayers][MAXN];
	for(int i = 0;i < numberOfPlayers; i++){
		scanf("%s", playerNames[i]);
	}
	initArena(&ar, numberOfPlayers, 1, playerNames);
	printArena(&ar);
	// puts ("\e[2J\e[1;1H");
	return 0;
}