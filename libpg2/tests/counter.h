#ifndef __COUNTER_H__
#define __COUNTER_H__

#include "7seg.h"

typedef struct Counter {
	int val;
	Display7Seg tens, units;
} Counter;



void Counter_init(Counter *c, int initial, int x, int y, int w, int h);

int Counter_dec(Counter *c);
 
int Counter_inc(Counter *c);
 

#endif
