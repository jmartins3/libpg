/*------------------------------------------
 O jogo do galo
 
 Jorge Martins, 2014
 
 -----------------------------------------*/
 
 
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "pg/pglib.h"

#include "tictactoe.h" 
 

 

void ttt_create_board(TTT_Board *ttt) {
	for (int i=0; i < SIDE; ++i) {
		 
		for (int j = 0; j < SIDE; j++) {
			ttt->board[i][j] = 0;
		}
		 
	}
	ttt->nplays = 0;
}



bool winner(TTT_Board *ttt, int player) {
	// check lines
	 
	for (int i=0; i < SIDE; ++i) {
		int j=0;
		for (; j < SIDE; j++) {
			if (ttt->board[i][j] != player) break;
		}
		if (j == SIDE) return true;
	}
	
	// check collumns
	for (int i=0; i < SIDE; ++i) {
		int j=0;
		for (; j < SIDE; j++) {
			if (ttt->board[j][i] != player) break;
		}
		if (j == SIDE) return true;
	}
	
	// check first diag
	int i;
	for (i=0; i < SIDE; ++i) {
		if (ttt->board[i][i] != player) break;
	}
	if (i== SIDE) return true;	
	
	// check second diag
 
	for (i=0; i < SIDE; ++i) {
		if (ttt->board[i][2-i] != player) break;
	}
	if (i== SIDE) return true;	
	return false;
}	


int ttt_play(TTT_Board *ttt, int x, int y, int piece) {
	if (ttt->board[x][y] != 0)
		return RESULT_BAD;
	 
	ttt->board[x][y] = piece;
	ttt->nplays++;
	if (piece == CROSS)
		draw_cross(x, y); 	
	else 
		draw_circle(x, y);
		
	if (winner(ttt, piece)) 
		return RESULT_WIN;
	else if (ttt->nplays == SIDE*SIDE)
		return RESULT_DRAW;
	else
		return RESULT_OK;
 
}


