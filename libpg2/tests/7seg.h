#ifndef __7SEG_H_
#define __7SEG_H_


typedef struct display7seg {
	int xo, yo;	// origin
	int width;
	int height;
} Display7Seg;

  
void d7s_display(Display7Seg *d, int val);
	 
void d7s_create(Display7Seg *d, int x, int y, int w, int h);
	 


#endif
