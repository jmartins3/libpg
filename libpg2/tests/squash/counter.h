#ifndef __COUNTER_H__
#define __COUNTER_H__


#include "7seg.h"

typedef struct  {
	int val;
	Display7Seg tens, units;
} counter_t;



void Counter_init(counter_t *c, int initial, int x, int y, int w, int h);

int Counter_dec(counter_t *c);
 
int Counter_inc(counter_t *c);
 

#endif
