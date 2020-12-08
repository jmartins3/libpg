#include <stdbool.h>
#include "pg/pglib.h"

#include "squash.h"
#include "7seg.h"


int segments[][8] = {
	{ 0, 1, 2, 4, 5, 6, -1 },		// 0
	{ 1, 4, -1 },					// 1
	{ 0, 2, 3, 4, 6, -1 },			// 2
	{ 0, 2, 3, 5, 6, -1 }, 			// 3
	{ 1, 2, 3, 5, -1  }, 			// 4
	{ 0, 1, 3, 5, 6 , -1 },			// 5
	{ 1, 3, 4, 5, 6, -1},			// 6
	{ 0, 2, 5, -1 }, 				// 7
	{ 0, 1, 2, 3, 4, 5, 6, -1}, 	// 8
	{ 0, 1, 2, 3, 5, -1 }			// 9
};


typedef struct seg {
	bool isVert;
	int cx,  cy;
} Seg;

Seg segPositions[] = {
	{ 0,  0, 0 },	// 0
	{ 1,  0, 0 },	// 1
	{ 1,  1, 0 },	// 2
	{ 0,  0, 1 },	// 3
	{ 1,  0, 1 },	// 4
	{ 1,  1, 1 }, 	// 5
	{ 0,  0, 2 },	// 6
};
		
void d7s_display_seg(Display7Seg *d, int si, RGB color) {
	if (si < 0 || si >  6) return;
	Seg seg = segPositions[si];
	int ofsx, ofsy;
	
	int segw = d->width  -8;
	int segh = d->height/2 - 6;
	
	int w, h;
	
	if (seg.isVert) {
		ofsx = seg.cx*segw + seg.cx*4;
		ofsy = seg.cy*segh + (seg.cy+1)*4;
		w=4; h = segh;
	}
	else {
		ofsx = 4;
		ofsy = seg.cy*segh +seg.cy*4;
		w=segw; h = 4;
	}


	 
	graph_rect( d->xo + ofsx, d->yo + ofsy,  w, h, color, true);
}


/* public interface */

void d7s_create(Display7Seg *d, int x, int y, int w, int h) {
	d->xo = x;
	d->yo = y;
	d->width = w;
	d->height = h;
}

static void d7s_hide(Display7Seg *d) {
	int seg, i=0;;
	int *digsegs = segments[8];
	while ((seg =  digsegs[i++]) != -1)
		d7s_display_seg(d, seg, border_color());
}

void d7s_display(Display7Seg *d, int val) {
	d7s_hide(d);
	int i=0, seg;
	if (val < 0 || val > 9) return;
	int *digsegs = segments[val];
	while ((seg =  digsegs[i++]) != -1)
		d7s_display_seg(d, seg, c_white);
}


	
