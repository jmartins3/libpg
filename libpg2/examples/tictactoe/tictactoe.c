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
int nplays;

/* 
 *  draw a paralelogram 
 *   
 */


void graph_paralelogram(int xc, int yc, int w, int h, int x0, RGB color, bool toFill) {
	
	if (x0 == 0 || x0 == -w) {
		graph_rect(xc, yc, w, h, color, toFill);
		return;
	}
	
	if (abs(x0) > w) 
		return;
	if ( x0 < 0) {
		
		x0 = -x0;
		
		if (toFill) {
			double m = (double) (xc - (xc + w - x0) ) / h;
			
			int ye = min(GRAPH_HEIGHT-1,yc+h-1);
			for (int y = yc; y <= ye; ++y) {
				int x = round((y - yc) * m + (xc + w - x0));
				
				// draw horizontal line between x1 and x2)
				graph_line(x, y, x + x0, y, color);
			}		
		}
		else {
			graph_line(xc + w -x0, yc, xc + w , yc, color);
			graph_line(xc + w -x0, yc, xc, yc + h, color);
			graph_line(xc, yc + h, xc + x0, yc + h, color);
			graph_line(xc+w, yc, xc + x0, yc + h, color);
		}
		
	}
	else {
		
		if (toFill) {
			double m = (double) ((xc + w - x0) - xc) / h;
			
			int ye = min(GRAPH_HEIGHT-1,yc+h-1);
			for (int y = yc; y <= ye; ++y) {
				int x = round((y - yc) * m + xc);
				
				// draw horizontal line between x1 and x2)
				graph_line(x, y, x + x0, y, color);
			}
		}
		else {
			graph_line(xc, yc, xc + x0 , yc, color);
			graph_line(xc, yc, xc + w-x0, yc + h, color);
			graph_line(xc+w-x0, yc + h, xc+ w, yc + h, color);
			graph_line(xc+x0, yc, xc + w, yc + h, color);
		}
		 
	}
} 


void ttt_draw_board() {
	
	graph_rect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT, c_white, true);
	int third_size_x = BOARD_WIDTH/3;
	int third_size_y = BOARD_HEIGHT/3;
	
	// draw board grid vertical lines
#ifdef GRAPH_SIMPLE
	graph_line(BOARD_LEFT_CORNER_X + third_size_x, BOARD_LEFT_CORNER_Y, 
			   BOARD_LEFT_CORNER_X + third_size_x, BOARD_LEFT_CORNER_Y + BOARD_HEIGHT, 
			   c_blue);
	graph_line(BOARD_LEFT_CORNER_X + 2*third_size_x, BOARD_LEFT_CORNER_Y, 
			   BOARD_LEFT_CORNER_X + 2*third_size_x, BOARD_LEFT_CORNER_Y + BOARD_HEIGHT, 
			   c_blue);	
#else
	   
	graph_rect(BOARD_LEFT_CORNER_X + third_size_x, BOARD_LEFT_CORNER_Y, 
			   GRID_WEIGHT, BOARD_HEIGHT, c_blue, true);
	graph_rect(BOARD_LEFT_CORNER_X + 2*third_size_x, BOARD_LEFT_CORNER_Y, 
			   GRID_WEIGHT, BOARD_HEIGHT, c_blue, true);
#endif

	// draw board grid horizontal lines


#ifdef GRAPH_SIMPLE
	graph_line(BOARD_LEFT_CORNER_X, BOARD_LEFT_CORNER_Y+third_size_y, 
			   BOARD_LEFT_CORNER_X+  BOARD_WIDTH, BOARD_LEFT_CORNER_Y+third_size_y, 
			   c_blue);
	graph_line(BOARD_LEFT_CORNER_X, BOARD_LEFT_CORNER_Y+2*third_size_y, 
			   BOARD_LEFT_CORNER_X + BOARD_WIDTH, BOARD_LEFT_CORNER_Y +2*third_size_y, 
			   c_blue);
#else
 
	graph_rect(BOARD_LEFT_CORNER_X, BOARD_LEFT_CORNER_Y+third_size_y, 
			   BOARD_WIDTH, GRID_WEIGHT, c_blue, true);
	graph_rect(BOARD_LEFT_CORNER_X, BOARD_LEFT_CORNER_Y + 2*third_size_y, 
			   BOARD_WIDTH, GRID_WEIGHT, c_blue, true);
#endif
}
	
void draw_cross(int x, int y) {
	int third_size_x = BOARD_WIDTH/3;
	int third_size_y = BOARD_HEIGHT/3;
	
	if (x < 0 || x > 2 || y < 0 || y >2) return;

#ifdef GRAPH_SIMPLE
	graph_line(BOARD_LEFT_CORNER_X +  third_size_x *x + SQUARE_BORDER,
				BOARD_LEFT_CORNER_Y + third_size_y *y + SQUARE_BORDER,
				BOARD_LEFT_CORNER_X +  third_size_x*(x +1) - SQUARE_BORDER,
				BOARD_LEFT_CORNER_Y + third_size_y*(y+1) - SQUARE_BORDER, 
				c_blue);
	
	graph_line(BOARD_LEFT_CORNER_X +  third_size_x *x + SQUARE_BORDER,
				BOARD_LEFT_CORNER_Y + third_size_y*(y+1) - SQUARE_BORDER,
				BOARD_LEFT_CORNER_X + third_size_x*(x+1) - SQUARE_BORDER,
				BOARD_LEFT_CORNER_Y + third_size_y *y + SQUARE_BORDER,
				c_blue);
	
#else					
	graph_paralelogram(BOARD_LEFT_CORNER_X + third_size_x*x + SQUARE_BORDER,
					   BOARD_LEFT_CORNER_Y + third_size_y*y + SQUARE_BORDER,
					   third_size_x - 2* SQUARE_BORDER,
					   third_size_y - 2* SQUARE_BORDER,
					   
					   SQUARE_WEIGHT, 
					   c_red, true);
	graph_paralelogram(BOARD_LEFT_CORNER_X + third_size_x*x + SQUARE_BORDER,
					   BOARD_LEFT_CORNER_Y + third_size_y*y + + SQUARE_BORDER,
					   third_size_x - 2* SQUARE_BORDER, 
					   third_size_y - 2* SQUARE_BORDER,
					   -SQUARE_WEIGHT,
					   c_red, true);
					   
#endif
}


void draw_circle(int x, int y) {
	int third_size_x = BOARD_WIDTH/3;
	int third_size_y = BOARD_HEIGHT/3;
	
	if (x < 0 || x > 2 || y < 0 || y >2) return;

#ifdef GRAPH_SIMPLE
	graph_circle(BOARD_LEFT_CORNER_X + third_size_x*x + third_size_x/2+SQUARE_WEIGHT/2,
			     BOARD_LEFT_CORNER_Y + third_size_y*y + third_size_y/2+SQUARE_WEIGHT/2,
			     third_size_x /2-2*SQUARE_WEIGHT, c_orange, false);
	
#else	     
	graph_circle(BOARD_LEFT_CORNER_X + third_size_x*x + third_size_x/2+SQUARE_WEIGHT/2,
			     BOARD_LEFT_CORNER_Y + third_size_y*y + third_size_y/2+SQUARE_WEIGHT/2,
			     third_size_x /2-2*SQUARE_WEIGHT, c_orange, true);
	graph_circle(BOARD_LEFT_CORNER_X + third_size_x*x + third_size_x/2+SQUARE_WEIGHT/2,
			     BOARD_LEFT_CORNER_Y + third_size_y*y + third_size_y/2+SQUARE_WEIGHT/2,
			     third_size_x / 2-3*SQUARE_WEIGHT, c_white, true);
#endif	     
	 	
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

static bool done;

void myMouseEventHandler(MouseEvent me) {
	static int turn=CROSS;
	
	
	if (me.type == MOUSE_BUTTON_EVENT && me.state == BUTTON_CLICK) {
		if (done == true) {
			graph_exit();
			return;
		}
		int third_size_x = BOARD_WIDTH/3+1;
		int third_size_y = BOARD_HEIGHT/3+1;
	
	
		int x = (me.x-BOARD_LEFT_CORNER_X);
		if ( x < 0) return;
		x = x /third_size_x;

		int y = (me.y-BOARD_LEFT_CORNER_Y);
		if ( y < 0) return;
		y = y /third_size_y;

		if (y < 0 || x < 0 || y > 2 || x > 2 || theBoard[x][y] != 0) return;
		
		
		theBoard[x][y] 	= turn;

		if (turn == CROSS)
			draw_cross(x, y); 	
		else 
			draw_circle(x, y);
		
		nplays++;
		if (winner(turn)) {
			printf("player with %s won!\n", turn == CROSS ? "cross" : "ball");
		    done = true;
		
		}
		else if (nplays == 9) {	
			printf("draw!\n");
			graph_exit();
		}
		
		turn = turn % 2 + 1;
		
	}
}



int main() {
	 
	graph_init();
	graph_regist_mouse_handler(myMouseEventHandler);
	ttt_draw_board();
	
	
	graph_start();
	 
}
	
  
