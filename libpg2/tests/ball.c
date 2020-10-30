#include "squash.h"

static void ball_show(Ball *b) {
	graph_circle(b->x, b->y, BALL_RADIUS, c_orange, true);
}

static void ball_advance(Ball *b) {
	b->x += b->dx;
	b->y += b->dy;
}


static void ball_erase(Ball *b) {
	graph_circle(b->x, b->y, BALL_RADIUS, floor_color(), true);
}

 

void ball_creat(Ball *b, int x, int y) {
	b->x = x; b->y = y;
	b->dx = b->dy = BALL_STEP;
	ball_show(b);
}


void ball_reverse_x(Ball *b) {
		b->dx = -b->dx;
}

void ball_reverse_y(Ball *b) {
		b->dy = -b->dy;
}

void ball_reverse(Ball *b) {
	ball_reverse_y(b);
	ball_reverse_x(b);
}

void ball_setPos(Ball *b, int x, int y, int visibility) {
	ball_erase(b);
	b->x = x;
	b->y = y;
	b->dx = b->dy = BALL_STEP;
	if (visibility)
		ball_show(b);
		
}

static int mabs(int v) {
	return v < 0 ? -v : v;
}

int  intersects(Ball *b, Racket *r) {
	int bx = b->x + b->dx, by = b->y + b->dy;
	
	if ((bx ) <= r->x + RACKET_WIDTH)  {
		// intersection on X axis
		if ((by + BALL_RADIUS/2) < r->y 						||
			(by - BALL_RADIUS/2) > (r->y + r->height))
			return -1;
		// adjust ball coordinates
		b->x = r->x + RACKET_WIDTH + BALL_RADIUS;
		return 1;
	}
	return 0;
	 
}


void ball_move(Ball *b, Racket *r) {
	 
	ball_erase(b);
	int res;
	
	res = intersects(b, r);
	if (res == 1) {
		sound_play("baseball_hit.wav"); 
		ball_reverse_x(b);
	}
	else if (res == -1) {
		loosePoint();
		return;
	}
	if (b->x + mabs(b->dx) +  BALL_RADIUS >= BOARD_RIGHT) {
		b->x = BOARD_RIGHT-1 - BALL_RADIUS;
		ball_reverse_x(b);
	}
	
	if ( b->y - mabs(b->dy) -  BALL_RADIUS <= BOARD_TOP) {
		b->y = BOARD_TOP + BALL_RADIUS+1; 
		ball_reverse_y(b);
	}
	else if ( b->y + mabs(b->dy + BALL_RADIUS) >= BOARD_BOTTOM) {
		b->y = BOARD_BOTTOM - BALL_RADIUS-1; 
		ball_reverse_y(b);
	}	
	ball_advance(b);
	ball_show(b);

}
