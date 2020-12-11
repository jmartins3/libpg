#ifndef _BALL_H_
#define _BALL_H_



#define BALL_RADIUS 10
#define BALL_STEP (2*BALL_RADIUS)

typedef struct Ball {
	int x,  y;
	int dx, dy;
} Ball;

void ball_reverse_x(Ball *b);

void ball_setPos(Ball *b, int x, int y, int visibility);	 

void ball_reverse_y(Ball *b);


void ball_reverse(Ball *b);

void ball_creat(Ball *r, int x, int y);
void ball_move(Ball *b, Racket *r);
	
#endif
