#include "pg/graphics.h"
#include "clock.h"

Clock create_clock(int x, int y, RGB color, RGB back) {
	Clock c;
	c.min = c.sec = 0;
	c.x = x; c.y = y;
	c.color =color;
	c.back=back;
	return c;
}

void draw_clock(Clock c) {
	char cstr[20];
	sprintf(cstr, "%02d:%02d", c.min, c.sec);
	graph_rect(c.x, c.y-24, 22*strlen(cstr), 24, c.back, true);
	graph_text(c.x, c.y, c.color, cstr, LARGE_FONT);
 
}

Clock tick(Clock c) {
	c.sec++;
	if (c.sec >= 60) {
		c.sec=0;
		c.min++;
		if (c.min >= 60) c.min =0;
	}
	return c;
}

