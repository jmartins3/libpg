/*------------------------------------------
 O jogo do galo
 
 Jorge Martins, 2014
 
 -----------------------------------------*/
 
 
#include <stdio.h>
#include <stdbool.h>
#include <math.h>


#include "pg/pglib.h"
#include "tictactoe.h" 
 

/* O modelo */

#define BALL	1
#define CROSS	2


typedef int TTT_Board[3][3];

#define BOARD_LEFT_CORNER_X	90
#define BOARD_LEFT_CORNER_Y	20

#define GRID_WEIGHT	10
#define SQUARE_BORDER 30
#define SQUARE_WEIGHT	12
#define BOARD_WIDTH	500
#define BOARD_HEIGHT 440


#define min(x,y) ((x) < (y) ? (x) : (y))


TTT_Board theBoard;

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

void myMouseEventHandler(MouseEvent me) {
	static int turn=CROSS;
	
	
	if (me.type == MOUSE_BUTTON_EVENT && me.state == BUTTON_CLICK) {
		if (state == SessionClosed) {
			graph_exit();
			return;
		}
		else {
			Point bp;
			 
			
			if (state != InGame ||  turn != MY_TURN || 
			    !screen_to_board(me.x, me.y, &bp)) {
				return;  // cannot play
			}
			 
			int res = ttt_play(&theBoard, bp.x, bp.y, my_piece);
			if (res != RESULT_BAD) {
				send_play(bp.x, bp.y);
				process_play(res, my_piece);
			}
		}
		
	}
}



int main() {
	 
	graph_init();
	graph_regist_mouse_handler(myMouseEventHandler);
	ttt_draw_board();
	
	
	graph_start();
	 
}
	
