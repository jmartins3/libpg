#include "squash.h"

 
static void racket_erase(Racket *r) {
	graph_rect(r->x, r->y, RACKET_WIDTH, r->height, floor_color(), true);
}

static void racket_show(Racket *r) {
	graph_rect(r->x, r->y, RACKET_WIDTH, r->height, c_blue, true);
}
 
static void racket_advance(Racket *r, int dy) {
	r->y += dy;
	if (r->y + r ->height >= BOARD_BOTTOM) 
		r->y = BOARD_BOTTOM - r->height;
	else if (r->y <= BOARD_TOP)
		r->y = BOARD_TOP;
}

void racket_creat(Racket *r, int x, int y) {
	r->x = x; r->y = y;
	r->height = RACKET_MAX_HEIGHT;
	racket_show(r);
}



void racket_move(Racket *r, uint key) {
	int rdelta; 
	if (key == SDLK_DOWN)  
		rdelta = RACKET_DELTA;
	else if (key == SDLK_UP) 
		rdelta = -RACKET_DELTA; 	 
		 
	racket_erase(r);
	racket_advance(r, rdelta);
	racket_show(r);
 
}
