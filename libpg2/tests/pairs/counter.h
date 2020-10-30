#include "pg/graphics.h"

struct counter {
	int x, y;
	RGB color, back;
	int val;
};
typedef struct counter Counter;

Counter create_counter(int x, int y, RGB color, RGB back);
	 
void draw_counter(Counter c);
	 

Counter inc(Counter c);
	 

