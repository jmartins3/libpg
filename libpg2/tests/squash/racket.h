#ifndef _RACKET_H
#define _RACKET_H
 
#define RACKET_WIDTH 		20
#define RACKET_MAX_HEIGHT 	90
#define RACKET_MIN_HEIGHT 	50

#define RACKET_DELTA 		25


typedef struct racket {
	int x, y;
	int height;
} Racket;


void racket_creat(Racket *r, int x, int y);
	 
void racket_move(Racket *r, uint key);
	 


#endif
