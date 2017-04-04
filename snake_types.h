#ifndef SNAKE_TYPES

#define ROWS 50 //size of arena
#define COLS 120
#define MAXN 50 //max length of snake

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
	int mSpeed;  //speed???
	int mScore;
}snake;

typedef struct arena{
	pair mFood[MAXN];
	char mGrid[ROWS][COLS];
	int mLevel;
	int mNumplayers;
	snake mSnakes[MAXN];
}arena;

#endif
