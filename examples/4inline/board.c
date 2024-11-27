#include <stdbool.h>
#include "pg/pglib.h"
#include "4inline.h"

bool winner(FourBoard *board, int player) {
	// check lines 
	for (int i=0; i < NLINES; ++i) {
		int count=0;
		for (int j = 0; j < NCOLS; ++j) {
			if (board->holes[i][j].color == player) {
				++count;
				if (count == 4) return true;
			}
			else count = 0;
		}
	}
	
	// check cols 
	for (int i=0; i < NLINES; ++i) {
		int count=0;
		for (int j = 0; j < NCOLS; ++j) {
			if (board->holes[j][i].color  == player) {
				++count;
				if (count == 4) return true;
			}
			else count = 0;
		}
	}
	
	// check diags 
	for (int i=0; i < 4; ++i) {
		int count1 = 0;
		for (int j =0; j + i < NCOLS ; ++j) {
			if (board->holes[j][j+i].color  == player) {
				++count1;
				if (count1 == 4) return true;
			}
			else count1 = 0;
		}
	}		
	
	for (int i=0; i < 4; ++i) {
		int count1 = 0;
		for (int j =0; j + i < NLINES; ++j) {
			if (board->holes[j+i][j].color  == player) {
				++count1;
				if (count1 == 4) return true;
			}
			else count1 = 0;
		}
	}		
	
	for (int i=NCOLS-1; i >= 3; --i) {
		int count1 = 0;
		for (int j =0;  i  - j  >= 0   ; ++j) {
			if (board->holes[j][i - j].color  == player) {
				++count1;
				if (count1 == 4) return true;
			}
			else count1 = 0;
			
		}
	}
	
	for (int i=NCOLS-1; i >= 3; --i) {
		int count1 = 0;
		for (int j =0;  i  - j  >= 0 && (NCOLS -1) - i + j < NLINES ; ++j) {
			 
			if (board->holes[(NCOLS -1) -i + j][NCOLS -1 - j].color  == player) {
				++count1;
				if (count1 == 4) return true;
			}
			else count1 = 0;
		}
	}
	return false;
}		

bool is_draw(FourBoard *board) {
	for (int i=0; i < NCOLS ; ++i)
		if (board->height[i] < NLINES) return false;
	return true;
}



void PointInit(Point *p, int x, int y) {
	p->x = x; p->y = y;
}

void HoleInit(Hole *h, Point p, int x, int y) {
	h->color = 0;
	h->center = p;
	h->x = x;
	h->y = y;
}
