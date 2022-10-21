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
int game_over;





int getCollumn(int x, int y) {
	if (x < BOARD_CORNER_X || x > BOARD_CORNER_X + BOARD_WIDTH ||
	    y < BOARD_CORNER_Y || y > BOARD_CORNER_Y + BOARD_HEIGHT) return -1;
	
	  
	return (x - BOARD_CORNER_X) / HOLE_WIDTH;
}

bool add_piece(int col, int color) {
	int top = theBoard.height[col];
	if (top >= NLINES) return false; 
	
	theBoard.holes[NLINES - top -1][col].color = color;
    theBoard.height[col]++;
	return true;
}

bool draw_piece(int line, int col) {
    Hole hole = theBoard.holes[NLINES-line-1][col];
	Point cp = hole.center;
	
	graph_circle(cp.x, cp.y, HOLE_RADIUS, hole.color == YELLOW ? c_yellow : c_red, true);
	return true;
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

static void fourl_draw_pieces() {
 	
	for (int i=0; i < NCOLS; ++i) {
		for (int j = 0; j < theBoard.height[i]; ++j) {
			 draw_piece(j, i);
		}
	}
	
}


void myMouseEventHandler(MouseEvent me) {
	static int turn = YELLOW;
	
	if (game_over == true) return;
	if (me.type == MOUSE_BUTTON_EVENT && me.state == BUTTON_PRESSED) {
		int col = getCollumn(me.x, me.y);
		if (col != -1 && add_piece(col, turn)) {
			
			if (winner(&theBoard, turn)) {
				printf("player %s wins the game!\n", turn == YELLOW ? "yellow" : "red");
				game_over=true;
				printf("press key to continue\n");
			}	
			else if (is_draw(&theBoard)) {
				printf("the game end with a draw!\n");
				game_over=true;
				printf("press key to continue\n");
			}
			turn = turn % 2 + 1;
		}
	}
}
		
void myKeyEventHandler(KeyEvent ke) {
	if (game_over)
		graph_exit();
		 
}

void ontick() {
    fourl_draw_pieces();
}
    


int main()  {
	graph_init2("Quatro em Linha", GRAPH_WIDTH, GRAPH_HEIGHT);
	
	graph_regist_mouse_handler(myMouseEventHandler);
	
	graph_regist_key_handler(myKeyEventHandler);
    
    graph_regist_timer_handler(ontick, 25);

	 
	fourl_draw_board();
	
	 
	graph_start();
	return 0;
}
	
  

	





