#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define CLOCKWISE 0
#define COUNTERCLOCKWISE 1

struct snakeDot {
	int x, y;
	struct snakeDot *next, *prev;
};

struct snake {
	struct snakeDot *head, *tail;
	/* CLOCKWISE/COUNTERCLOCKWISE */
	int direction;
	/* UP/RIGHT/DOWN/LEFT */
	int currentDirection;
	int size;
	int boardSize;
	int directionChangeCount;
	int modifier;
	int modifierSign;
	int modifierChangeCounter;
	int *headPositionCache;
	int freezeCounter;
};

struct snake* getSnake(int,int);
void clean(struct snake*);
int move(struct snake*);
void changeDirection(struct snake*);
void display(struct snake*,int);

int main(int argc, char* argv[]) {

	struct snake *snake = getSnake(3, 5);

	display(snake, 1);
	usleep(1000000);
	while(move(snake)!=-1) {
		display(snake,-1);
		usleep(100000);
	}
	display(snake, 1);
	usleep(1000000);

	clean(snake);

	return 0;
}

struct snake* getSnake(int size, int boardSize) {
	int i;
	struct snake* snake;
	struct snakeDot *head;
	struct snakeDot *prev, *curr;

	snake = malloc(sizeof(struct snake));
	head = malloc(sizeof(struct snakeDot));

	head->x=0;
	head->y=0;
	prev = head;
	for(i=0;i<size-1;++i) {
		curr = malloc(sizeof(struct snakeDot));
		prev->next = curr;
		curr->prev = prev;
		curr->x = prev->x-1;
		curr->y = prev->y-1;
		prev = curr;
	}
	snake->tail = prev;

	snake->direction = CLOCKWISE;
	snake->currentDirection = RIGHT;
	snake->size = size;
	snake->head = head;
	snake->boardSize = boardSize;
	snake->directionChangeCount = 0;
	snake->modifierSign = 1;
	snake->modifierChangeCounter = 0;
	snake->freezeCounter = 0;
	return snake;
}

void clean(struct snake *snake) {
	struct snakeDot *curr,*next;
	curr = snake->head;
	while(curr!=NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
	free(snake);
}

int oppositeDirection(int dir) {
	if(dir == LEFT)return RIGHT;
	if(dir == RIGHT)return LEFT;
	if(dir == UP)return DOWN;
	if(dir == DOWN)return UP;
	return -1;
}

void changeDirection(struct snake *snake) {
	int modifier = snake->directionChangeCount/3;
	switch(snake->currentDirection) {
		case RIGHT: {
			if(snake->head->x >= snake->boardSize-1-modifier) {
				snake->head->x = snake->boardSize-1-modifier;
				snake->currentDirection = (snake->direction==CLOCKWISE) ?
						DOWN : UP ;
				snake->directionChangeCount += snake->modifierSign;
			}
			break;
		}
		case UP: {
			if(snake->head->y <= modifier) {
				snake->head->y = modifier;
				snake->currentDirection = (snake->direction==CLOCKWISE) ?
						RIGHT : LEFT ;
				snake->directionChangeCount += snake->modifierSign;
			}
			break;
		}
		case DOWN: {
			if(snake->head->y >= snake->boardSize-1-modifier) {
				snake->head->y = snake->boardSize-1-modifier;
				snake->currentDirection = (snake->direction==CLOCKWISE) ?
						LEFT : RIGHT ;
				snake->directionChangeCount += snake->modifierSign;
			}
			break;
		}
		case LEFT: {
			if(snake->head->x <= modifier) {
				snake->head->x = modifier;
				snake->currentDirection = (snake->direction==CLOCKWISE) ?
						UP : DOWN ;
				snake->directionChangeCount += snake->modifierSign;
			}
			break;
		}
	}
}

int move(struct snake *snake) {
	struct snakeDot *curr;
	if(snake->boardSize/2 < snake->directionChangeCount/3) {
		snake->direction = oppositeDirection(snake->direction);
		snake->modifierSign *= -1;
	}
	curr = snake->tail;
	while(curr!=snake->head) {
		curr->x = curr->prev->x;
		curr->y = curr->prev->y;
		curr = curr->prev;
	}

	switch(snake->currentDirection) {
		case RIGHT: {
			++snake->head->x;
			break;
		}
		case UP: {
			--snake->head->y;
			break;
		}
		case DOWN: {
			++snake->head->y;
			break;
		}
		case LEFT: {
			--snake->head->x;
			break;
		}
	}

	
	changeDirection(snake);

	if(snake->head->x == 0 && snake->head->y == 0) {
		return -1;
	}

	return 0;
}

int getFieldCode(struct snake *snake, int x, int y) {
	struct snakeDot *curr = snake->head;
	while(curr!=NULL) {
		if(curr->x == x && curr->y == y) return 1;
		curr = curr->next;
	} 
	return 0;
}

void display(struct snake *snake, int force) {
	int i,j,x;
	system("clear");
	for(i=0;i<snake->boardSize;++i) {
		for(j=0;j<snake->boardSize;++j) {
			x = (force < 0) ? getFieldCode(snake,i,j) : force ;
			printf("%d", x);
		}
		printf("\n");
	}
	printf("\n");
	printf("{%d} [%d,%d] {%d}\n", snake->directionChangeCount, snake->head->x, snake->head->y, snake->freezeCounter);
}