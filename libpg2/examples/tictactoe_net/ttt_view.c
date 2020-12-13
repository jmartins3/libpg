#include "pg/pglib.h"

#include <stdbool.h>
#include "tictactoe.h"


 
/* 
 *  draw a paralelogram 
 *   
 */

static RGB back_color;
 

static void graph_paralelogram(int xc, int yc, int w, int h, int x0, RGB color, bool toFill) {
	
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
			
			int ye = min(WINDOW_HEIGHT-1,yc+h-1);
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
			
			int ye = min(WINDOW_HEIGHT-1,yc+h-1);
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

void ttt_draw_board(RGB bc) {
	
	
	int third_size_x = BOARD_WIDTH/3;
	int third_size_y = BOARD_HEIGHT/3;
	
	back_color = bc;
	// draw board grid vertical lines
#ifdef GRAPH_SIMPLE
	graph_line(BOARD_X + third_size_x, BOARD_Y, 
			   BOARD_X + third_size_x, BOARD_Y + BOARD_HEIGHT, 
			   c_blue);
	graph_line(BOARD_X + 2*third_size_x, BOARD_Y, 
			   BOARD_X + 2*third_size_x, BOARD_Y + BOARD_HEIGHT, 
			   c_blue);	
#else
	   
	graph_rect(BOARD_X + third_size_x, BOARD_Y, 
			   GRID_WEIGHT, BOARD_HEIGHT, c_blue, true);
	graph_rect(BOARD_X + 2*third_size_x, BOARD_Y, 
			   GRID_WEIGHT, BOARD_HEIGHT, c_blue, true);
#endif

	// draw board grid horizontal lines


#ifdef GRAPH_SIMPLE
	graph_line(BOARD_X, BOARD_Y+third_size_y, 
			   BOARD_X+  BOARD_WIDTH, BOARD_Y+third_size_y, 
			   c_blue);
	graph_line(BOARD_X, BOARD_Y+2*third_size_y, 
			   BOARD_X + BOARD_WIDTH, BOARD_Y +2*third_size_y, 
			   c_blue);
#else
 
	graph_rect(BOARD_X, BOARD_Y+third_size_y, 
			   BOARD_WIDTH, GRID_WEIGHT, c_blue, true);
	graph_rect(BOARD_X, BOARD_Y + 2*third_size_y, 
			   BOARD_WIDTH, GRID_WEIGHT, c_blue, true);
#endif
}
	
void draw_cross(int x, int y) {
	int third_size_x = BOARD_WIDTH/3;
	int third_size_y = BOARD_HEIGHT/3;
	
	if (x < 0 || x > 2 || y < 0 || y >2) return;

#ifdef GRAPH_SIMPLE
	graph_line(BOARD_X +  third_size_x *x + SQUARE_BORDER,
				BOARD_Y + third_size_y *y + SQUARE_BORDER,
				BOARD_X +  third_size_x*(x +1) - SQUARE_BORDER,
				BOARD_Y + third_size_y*(y+1) - SQUARE_BORDER, 
				c_blue);
	
	graph_line(BOARD_X +  third_size_x *x + SQUARE_BORDER,
				BOARD_Y + third_size_y*(y+1) - SQUARE_BORDER,
				BOARD_X + third_size_x*(x+1) - SQUARE_BORDER,
				BOARD_Y + third_size_y *y + SQUARE_BORDER,
				c_blue);
	
#else					
	graph_paralelogram(BOARD_X + third_size_x*x + SQUARE_BORDER,
					   BOARD_Y + third_size_y*y + SQUARE_BORDER,
					   third_size_x - 2* SQUARE_BORDER,
					   third_size_y - 2* SQUARE_BORDER,
					   
					   SQUARE_WEIGHT, 
					   c_red, true);
	graph_paralelogram(BOARD_X + third_size_x*x + SQUARE_BORDER,
					   BOARD_Y + third_size_y*y + + SQUARE_BORDER,
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
	graph_circle(BOARD_X + third_size_x*x + third_size_x/2+SQUARE_WEIGHT/2,
			     BOARD_Y + third_size_y*y + third_size_y/2+SQUARE_WEIGHT/2,
			     third_size_x /2-2*SQUARE_WEIGHT, c_orange, false);
	
#else	     
	graph_circle(BOARD_X + third_size_x*x + third_size_x/2+SQUARE_WEIGHT/2,
			     BOARD_Y + third_size_y*y + third_size_y/2+SQUARE_WEIGHT/2,
			     third_size_x /2-2*SQUARE_WEIGHT, c_orange, true);
	graph_circle(BOARD_X + third_size_x*x + third_size_x/2+SQUARE_WEIGHT/2,
			     BOARD_Y + third_size_y*y + third_size_y/2+SQUARE_WEIGHT/2,
			     third_size_x / 2-3*SQUARE_WEIGHT, back_color, true);
#endif	     
	 	
}
