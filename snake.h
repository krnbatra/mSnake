void initSnake(snake* sn, char name[]);

void placeFood(arena* ar, int numFood);

void initArena(arena* ar, int numberOfPlayers, int level, char names[][MAXN]);

void printArena(arena* ar);

void moveSnake(snake* sn, char key);
