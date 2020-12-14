/*------------------------------------------
 O jogo do galo
 
 Jorge Martins, 2014
 
 -----------------------------------------*/
 
 
#include <stdio.h>
#include <stdbool.h>
#include <math.h>


#include "pg/pglib.h"

#include "ttt_board.h"
#include "ttt.h" 
 

// variáveis globais 

bool game_over;

TTT_Board theBoard;
MsgView status_msg;
int turn;

void show_victory_message(int piece) {
	char msg[128];
	
	sprintf(msg, "%s Win!", piece == CROSS ? "Cross" : "Ball");
	mv_show_text(&status_msg, msg, ALIGN_CENTER);
}
 

void show_draw_message() {
	mv_show_text(&status_msg, "Draw!", ALIGN_CENTER);
}



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

void mouse_handler(MouseEvent me) {	
	
	if (me.type == MOUSE_BUTTON_EVENT && me.state == BUTTON_CLICK) {
		if (game_over) {
			graph_exit();
			return;
		}
		else {
			Point bp;
			 
			
			if ( !screen_to_board(me.x, me.y, &bp)) {
				return;  // cannot play
			}
			 
			int res = ttt_play(&theBoard, bp.x, bp.y, turn);
			switch(res) {
				case PLAY_WIN:
					show_victory_message(turn);
					game_over = true;
					break;
				case PLAY_DRAW:
					show_draw_message();
					game_over = true;
				case PLAY_NO_WIN:
					turn = (turn == CROSS) ? BALL : CROSS;
					break;
				default:
					break;
			}
					
				
		}
		
	}
}

void prepare_game() {
 	// draw background
	graph_rect(0,0, WINDOW_WIDTH, WINDOW_HEIGHT,  BACK_COLOR, true);
	
	ttt_create_board(&theBoard);
	ttt_draw_board(BACK_COLOR);
		
	 	
	mv_create(&status_msg, STATUS_MSG_X, STATUS_MSG_Y, STATUS_MSG_SIZE, STATUS_MSG_FONT,MSG_TC, MSG_BC);

	
	turn = CROSS;
	 
}
	
	

int main() {
	 
	graph_init2("Tic Tac Toe", WINDOW_WIDTH, WINDOW_HEIGHT);
	graph_regist_mouse_handler(mouse_handler);
	prepare_game();
	
	
	graph_start();
	 
}
	
