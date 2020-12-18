/*------------------------------------------
 O jogo do galo
 
 Jorge Martins, 2020
 
 -----------------------------------------*/
 
 
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "pg/pglib.h"


#include "ttt.h"
 

 

void ttt_create_board(ttt_board_t *ttt) {
	for (int i=0; i < SIDE; ++i) {
		 
		for (int j = 0; j < SIDE; j++) {
			ttt->board[i][j] = 0;
		}
		 
	}
	ttt->nplays = 0;
}



bool winner(ttt_board_t *ttt, int player) {
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


int ttt_play(ttt_board_t *ttt, int x, int y, int piece) {
	if (ttt->board[x][y] != 0)
		return PLAY_INVALID;
	 
	ttt->board[x][y] = piece;
	ttt->nplays++;
	
	if (winner(ttt, piece)) 
		return PLAY_WIN;
	else if (ttt->nplays == SIDE*SIDE)
		return PLAY_DRAW;
	else
		return PLAY_NO_WIN;
 
}


