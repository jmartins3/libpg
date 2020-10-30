#include "pg/graphics.h"
#include "counter.h"

 

Counter create_counter(int x, int y, RGB color, RGB back) {
	Counter c;
	c.val=  0;
	c.x = x; c.y = y;
	c.color = color;
	c.back =back;
	return c;
}

void draw_counter(Counter c) {
	char cstr[20];
	sprintf(cstr, "%3d", c.val);
	graph_rect(c.x, c.y-24, 22*strlen(cstr), 24, c.back, true);
	graph_text(c.x, c.y, c.color, cstr, LARGE_FONT);
 
	
}

Counter inc(Counter c) {
	c.val++;
	return c;
}
