/*------------------------------------------
 O jogo do galo
 
 Jorge Martins, 2014
 
 -----------------------------------------*/
 
 
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "pg/pglib.h"

#include "tictactoe.h" 
 

 
static TTT_Board theBoard;


bool screen_to_board(int mx, int my, Point *p) {
	int third_size_x = BOARD_WIDTH/3+1;
	int third_size_y = BOARD_HEIGHT/3+1;


	int x = (mx - BOARD_X);
	if ( x < 0) return false;
	x = x /third_size_x;

	int y = (my -BOARD_Y);
	if ( y < 0) return false;
	y = y /third_size_y;

	if (y < 0 || x < 0 || y > 2 || x > 2 ) {
		p->x = p->y = -1;
		return false;
	}
	p-> x  = x;
	p-> y  = y;
	return true;
}



void ttt_create_board() {
	
}



bool winner(int player) {
	// check lines
	 
	for (int i=0; i < 3; ++i) {
		int j=0;
		for (; j < 3; j++) {
			if (theBoard[i][j] != player) break;
		}
		if (j == 3) return true;
	}
	
	// check collumns
	for (int i=0; i < 3; ++i) {
		int j=0;
		for (; j < 3; j++) {
			if (theBoard[j][i] != player) break;
		}
		if (j == 3) return true;
	}
	
	// check first diag
	int i;
	for (i=0; i < 3; ++i) {
		if (theBoard[i][i] != player) break;
	}
	if (i== 3) return true;	
	
	// check second diag
 
	for (i=0; i < 3; ++i) {
		if (theBoard[i][2-i] != player) break;
	}
	if (i== 3) return true;	
	return false;
}	


bool ttt_play(int x, int y, int piece) {
	if (theBoard[x][y] != 0)
		return false;
	 
	theBoard[x][y] 	= piece;

	if (piece == CROSS)
		draw_cross(x, y); 	
	else 
		draw_circle(x, y);
	return true;
}


