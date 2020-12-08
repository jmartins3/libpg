#include "7seg.h"
#include "counter.h"



static void Counter_show(counter_t *c) {
	d7s_display(&c->tens, c->val /10);
	d7s_display(&c->units, c->val % 10);
}



void Counter_init(counter_t *c, int initial, int x, int y, int w, int h) {
	int display_w = w /2 -4;
	int display_h = h;
	
	int xd = x, yd = y;
	int xu = x + display_w + 8;
	int yu = y;
	
	d7s_create(&c->tens, xd, yd, display_w, display_h);
	d7s_create(&c->units, xu, yu, display_w, display_h);
	
	c->val = initial;
	
	Counter_show(c);
}



int Counter_dec(counter_t *c) {
	if (c->val > 0) 
		c->val--;
	Counter_show(c);
	return c->val;
}

int Counter_inc(counter_t *c) {
	c->val++;
	if (c->val > 99) 
		c->val =0;
		
	Counter_show(c);
	return c->val;
}

