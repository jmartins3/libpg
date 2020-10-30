#ifndef _GRAPHICS_H

#define GRAPHICS_H

#ifndef __cplusplus
#include 	<stdbool.h>
#endif

#include "events.h"
#include "audio.h"

/* Graph Window Dimensions */

#define GRAPH_WIDTH  640
#define GRAPH_HEIGHT 480

/* Event id for exit session */

#define END_SESSION	1

#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>



extern int __done;

typedef Uint32 RGB;

typedef unsigned char byte;



#ifdef LIBPG_IMPL
void timebase_regist();
extern SDL_Renderer *screen;
#endif





#ifdef __cplusplus 

extern "C" {
	
#endif


// returns an rgb color from components
RGB graph_rgb(int r, int g, int b);

// initializes the graphics. Must be called first	 
int graph_init();

// supsend execution for x miliseconds
void graph_delay(int ms);

// draws a pixel at position with specified color
void graph_pixel(short x, short y, RGB color);
               
// draws a line at position with specified color
void graph_line(short x1, short y1, short x2, short y2, RGB color);
	 
// draws a circle at position with specified color, filled or not
void  graph_circle(short x0, short y0, short radius, RGB color, bool toFill);
	            
// draws a rect at position with specified color, filled or not
void graph_rect(short x0, short y0, short w, short h, RGB color, bool toFill);
	 
	 
// draws a ellipse at position with specified color, filled or not
void  graph_ellipse(short x0, short y0, short xr, short yr, RGB color, bool toFill);


// draws a round rectangle at position with specified color, filled or not
void graph_round_rect(short x0, short y0, short w, short h, RGB color, bool toFill);

// draws a triangle at position (x0,y0) with specified color, filled or not
int graph_triangle2(int x0, int y0, int a, int b, int c, RGB color, bool toFill);

// draws a triangle given his points (x0,y0), (x1,y1), (x2,y2)
// with specified color, filled or not
void graph_triangle(int x0, int y0, int x1, int y1, int x2, int y2, RGB color, bool toFill );

// start the event loop
void graph_start();

// stop the event loop
void  graph_exit();	
	 
// refresh graphics windows. Must be called to show done operations result
void graph_refresh();


// Text Functions

//for text write
#define SMALL_FONT 	1
#define MEDIUM_FONT	2
#define LARGE_FONT	3

/*
 * Writes a string starting at point x,y with "color" as text color
 * and witth font size specified in fontsize (SMALL, MEDIUM, LARGE)
 */
void graph_text( short x, short y, RGB color, char text[], int fontsize );


// Image functions	 
bool graph_image(const char *path, int x, int y, int width, int height);
bool graph_save_screen(const char *path);

void graph_start_capture();
void graph_end_capture();
RGB  graph_get_pixel(int x, int y);
	 
// Sound functions


bool  sound_play(const char *path);
void sound_pause();
void sound_resume();
void sound_stop();
	 
#ifdef __cplusplus 

}
	
#endif


// colors

#define	c_white 		graph_rgb(255,255,255)
#define	c_lightblue 	graph_rgb(245,245,255)
#define	c_dgray 		graph_rgb(64,64,64)
#define	c_cyan 			graph_rgb(32,255,255)
#define	c_black			graph_rgb(0,0,0)
#define	c_gray			graph_rgb(210,210,210)
#define	c_red			graph_rgb(255, 0, 0)
#define	c_green			graph_rgb(0, 255, 0)
#define	c_blue			graph_rgb(0, 0, 255)
#define	c_orange		graph_rgb(255, 146, 36)

#endif
