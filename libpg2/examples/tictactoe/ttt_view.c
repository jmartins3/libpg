
#include <stdbool.h>
#include "pg/pglib.h"


#include "ttt.h"


#define min(x,y) ((x) < (y) ? (x) : (y))



static void graph_bold_line(int x0, int y0, int x1, int y1, int bold, RGB color) {
	int delta = (x0 -x1) < 0 ? 1 : -1;
	for (int i =0; i < bold; ++i) 
		graph_line(x0+ i*delta, y0, x1 +i*delta, y1, color);
}

 
void ttt_draw_board() {
	
	
	int third_size_x = BOARD_WIDTH/3;
	int third_size_y = BOARD_HEIGHT/3;
	
	// draw board grid vertical lines
 
	graph_rect(BOARD_X + third_size_x, BOARD_Y, 
			   BOARD_GRID_WEIGHT, BOARD_HEIGHT, BOARD_GRID_COLOR, true);
	graph_rect(BOARD_X + 2*third_size_x, BOARD_Y, 
			   BOARD_GRID_WEIGHT, BOARD_HEIGHT, BOARD_GRID_COLOR, true);
 

	// draw board grid horizontal lines
 
 
	graph_rect(BOARD_X, BOARD_Y+third_size_y, BOARD_WIDTH, 
				BOARD_GRID_WEIGHT, BOARD_GRID_COLOR, true);
			   
	graph_rect(BOARD_X, BOARD_Y + 2*third_size_y,  BOARD_WIDTH, 
				BOARD_GRID_WEIGHT, BOARD_GRID_COLOR, true);
 
}
	
void draw_cross(int x, int y) {
	const int margin = BOARD_CELL_SIDE/4;
	const int weight = 10;
	
	int wx = BOARD_X + x * BOARD_CELL_SIDE + margin;
	int wy = BOARD_Y + y * BOARD_CELL_SIDE + margin;
	
	graph_bold_line(wx, wy, wx + BOARD_CELL_SIDE - 2* margin,
					   wy + BOARD_CELL_SIDE - 2* margin, weight, CROSS_COLOR);
	graph_bold_line(wx + BOARD_CELL_SIDE - 2* margin, wy,
			   wx, wy + BOARD_CELL_SIDE - 2* margin, weight, CROSS_COLOR);
	
}
 

void draw_ball(int x, int y) {
	int wx = BOARD_X + x * BOARD_CELL_SIDE + BOARD_CELL_SIDE/2;
	int wy = BOARD_Y + y * BOARD_CELL_SIDE + BOARD_CELL_SIDE/2;
	int radius = (BOARD_CELL_SIDE - (BOARD_CELL_SIDE/4)) /2;
	graph_circle(wx, wy, radius, BALL_COLOR, true);
	graph_circle(wx, wy, radius - radius/5, BOARD_COLOR, true);
}


void draw_piece(int x, int y, int piece) {
	if (piece == CROSS)
		draw_cross(x, y); 	
	else 
		draw_ball(x, y);
		
}
