#include "pg/pglib.h"

int main() {
	graph_init();
 	
 	
 	graph_text(10, 30, c_blue, "Hello PG Graphics!", LARGE_FONT );
	graph_line(0,0,400, 400, c_green);
	graph_pixel(600, 20, c_black);
	graph_pixel(600, 21, c_black);
	graph_rect(400, 300, 60, 30, c_red, true);
	
	
	graph_circle(200, 100, 50, c_green, false);
	graph_circle(200, 150, 50, c_white, true);
	 
	graph_rect(80, 300, 60, 30, c_black, false);
	
	
	
	graph_ellipse(500, 400, 30, 50, c_red, true);
	
	graph_round_rect(500, 200,  60, 100, c_blue, false);
	
	graph_start();
	return 0;
}

