/******************************************************
 * Jogo do 4 em linha
 *****************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <math.h>


#include "pg/pglib.h"
#include "4inline.h"
 

/* globals */

 
FourBoard theBoard;
int game_ended;




void PointInit(Point *p, int x, int y) {
	p->x = x; p->y = y;
}

void HoleInit(Hole *h, Point p, int x, int y) {
	h->color = 0;
	h->center = p;
	h->x = x;
	h->y = y;
}


int getCollumn(int x, int y) {
	if (x < BOARD_CORNER_X || x > BOARD_CORNER_X + BOARD_WIDTH ||
	    y < BOARD_CORNER_Y || y > BOARD_CORNER_Y + BOARD_HEIGHT) return -1;
	
	  
	return (x - BOARD_CORNER_X) / HOLE_WIDTH;
}

bool draw_piece(int col, int color) {
	int top = theBoard.height[col];
	if (top >= NLINES) return false; 
	Point cp = theBoard.holes[NLINES - top -1][col].center;
	
	graph_circle(cp.x, cp.y, HOLE_RADIUS, color == YELLOW ? c_yellow : c_red, true);
	theBoard.holes[NLINES - top -1][col].color = color;
	graph_refresh();
	return true;
}


bool isdraw() {
	for (int i=0; i < NCOLS ; ++i)
		if (theBoard.height[i] < NLINES) return false;
	return true;
}

bool winner(int player) {
	// check lines 
	for (int i=0; i < NLINES; ++i) {
		int count=0;
		for (int j = 0; j < NCOLS; ++j) {
			if (theBoard.holes[i][j].color == player) {
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
			if (theBoard.holes[j][i].color  == player) {
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
			if (theBoard.holes[j][j+i].color  == player) {
				++count1;
				if (count1 == 4) return true;
			}
			else count1 = 0;
		}
	}		
	
	for (int i=0; i < 4; ++i) {
		int count1 = 0;
		for (int j =0; j + i < NLINES; ++j) {
			if (theBoard.holes[j+i][j].color  == player) {
				++count1;
				if (count1 == 4) return true;
			}
			else count1 = 0;
		}
	}		
	
	for (int i=NCOLS-1; i >= 3; --i) {
		int count1 = 0;
		for (int j =0;  i  - j  >= 0   ; ++j) {
			if (theBoard.holes[j][i - j].color  == player) {
				++count1;
				if (count1 == 4) return true;
			}
			else count1 = 0;
			
		}
	}
	
	for (int i=NCOLS-1; i >= 3; --i) {
		int count1 = 0;
		for (int j =0;  i  - j  >= 0 && (NCOLS -1) - i + j < NLINES ; ++j) {
			 
			if (theBoard.holes[(NCOLS -1) -i + j][NCOLS -1 - j].color  == player) {
				++count1;
				if (count1 == 4) return true;
			}
			else count1 = 0;
		}
	}
	return false;
}		
			
static void fourl_draw_board() {
	RGB c_brown = graph_rgb(148, 79, 58);
	RGB c_dark_brown = graph_rgb(75, 40, 30);
	
	graph_rect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT, c_black, true);
	graph_rect(BOARD_CORNER_X, BOARD_CORNER_Y, BOARD_WIDTH, BOARD_HEIGHT, c_brown, true);
	
	// draw holes
	// calculate hole area
	 
 	
	for (int i=0; i < NLINES; ++i) {
		for (int j = 0; j < NCOLS; ++j) {
			int center_x = BOARD_CORNER_X + HOLE_WIDTH * j + HOLE_WIDTH/2;
			int center_y = BOARD_CORNER_Y + HOLE_HEIGHT * i + HOLE_HEIGHT/2;
			
			Point p;
			PointInit(&p, center_x, center_y);
			graph_circle(center_x , center_y, HOLE_RADIUS, c_black, true);
			HoleInit(&theBoard.holes[i][j], p, i, j);
		}
	}
	
	// draw border
	graph_rect(BOARD_CORNER_X-2*HOLE_BORDER, BOARD_CORNER_Y + BOARD_HEIGHT, BOARD_WIDTH+4*HOLE_BORDER, 
				2*HOLE_BORDER, c_dark_brown, true);
	graph_rect(BOARD_CORNER_X-HOLE_BORDER/2, BOARD_CORNER_Y -HOLE_BORDER, HOLE_BORDER, 
				 BOARD_HEIGHT + HOLE_BORDER, c_dark_brown, true);
	graph_rect(BOARD_CORNER_X+BOARD_WIDTH-HOLE_BORDER/2, BOARD_CORNER_Y -HOLE_BORDER, HOLE_BORDER, 
				 BOARD_HEIGHT + HOLE_BORDER, c_dark_brown, true);
}

void myMouseEventHandler(MouseEvent me) {
	static int turn = YELLOW;
	
	if (game_ended == true) return;
	if (me.type == MOUSE_BUTTON_EVENT && me.state == BUTTON_PRESSED) {
		int col = getCollumn(me.x, me.y);
		if (col != -1 && draw_piece(col, turn)) {
			theBoard.height[col]++;
			
			if (winner(turn)) {
				printf("player %s wins the game!\n", turn == YELLOW ? "yellow" : "red");
				game_ended=true;
				printf("press key to continue\n");
			}	
			else if (isdraw()) {
				printf("the game end with a draw!\n");
				game_ended=true;
				printf("press key to continue\n");
			}
			turn = turn % 2 + 1;
		}
	}
}
		
void myKeyEventHandler(KeyEvent ke) {
	if (game_ended)
		graph_exit();
		 
}		


int main()  {
	graph_init2("Quatro em Linha", GRAPH_WIDTH, GRAPH_HEIGHT);
	
	graph_regist_mouse_handler(myMouseEventHandler);
	
	graph_regist_key_handler(myKeyEventHandler);

	 
	fourl_draw_board();
	
	 
	graph_refresh();
	 
	graph_start();
	return 0;
}
	
  

	





