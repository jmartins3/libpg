#define _XOPEN_SOURCE 500
#define LIBPG_IMPL

#include "../include/graphics.h"

#include 	<stdbool.h>
#include 	<pthread.h>
#include 	<signal.h>
#include 	<unistd.h>
#include 	"../include/audio.h"

#include 	"../include/socket_events.h"
#include 	"../include/uv_srv.h"
#include    "../include/img_cache.h"

// #define DEBUG_START

/* default graph Window Dimensions */

#define GRAPH_WIDTH  1024
#define GRAPH_HEIGHT 768

/*-------------------------------------
 * Globals
 *------------------------------------*/
SDL_Renderer *screen;

pthread_t ui_thread;

// used for loop interruption
volatile int __done;

static int window_height;
static int window_width;
	
static void interrupted(int s) {
	__done = 1;
}
  


static int internal_init(const char title[], bool with_timer, int width, int height) {
	SDL_Window *window= NULL;
		
	signal(SIGINT, interrupted);
	
	if ( SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
		printf("SDL initialization error: %s\n", SDL_GetError());
		return -1;
	}
 
    __USER_EVENTS = SDL_RegisterEvents(4);	
	if (__USER_EVENTS == (unsigned int) -1) {
		printf("SDL initialization error: %s\n", SDL_GetError());
		return  -1;	
	}		 
	
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
							  width, height, SDL_WINDOW_SHOWN);
	
	if (window == NULL) return -1;
	
	screen = SDL_CreateRenderer(window, -1, /* SDL_RENDERER_ACCELERATED */ SDL_RENDERER_SOFTWARE);
	
	SDL_SetRenderDrawColor(screen, 255,255,255,255);
	
	SDL_RenderClear(screen);
	
    SDL_RendererInfo rinfo;
    SDL_GetRendererInfo(screen, &rinfo);
    
    window_width = width;
    window_height = height;
    
    ui_thread = pthread_self();
	return 1;
}


void graph_delay(int ms) {
	usleep(ms*1000);
}

bool graph_image(const char *path, int x, int y, int width, int height) {
    
	SDL_Texture  *img = icache_get(path);
    if (img == NULL) {
        img = IMG_LoadTexture(screen, path);
        if (img == NULL) return false;
        icache_put(path, img);
    }
   
	SDL_Rect texr; texr.x = x; texr.y = y; 
	texr.w = width; texr.h = height; 
	SDL_RenderCopy(screen, img, NULL, &texr);
    //SDL_DestroyTexture(img);
	return true;
}

static SDL_Surface *sshot;

void graph_start_capture() {
	sshot = SDL_CreateRGBSurface(0, graph_get_width(), graph_get_height(), 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(screen, NULL, SDL_PIXELFORMAT_ABGR8888, sshot->pixels, sshot->pitch);
}

ScrArea graph_start_capture2(Rect r) {
	ScrArea area;
	area = SDL_CreateRGBSurface(0, r.sz.width, r.sz.height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(screen, (SDL_Rect*) &r, SDL_PIXELFORMAT_ABGR8888, area->pixels, area->pitch);
	return area;
}

void graph_end_capture() {
	SDL_FreeSurface(sshot);
	sshot = NULL;
}


void graph_end_capture2(ScrArea area) {
	SDL_FreeSurface(area);
}

RGB graph_get_pixel2(ScrArea area, int x, int y) {
	if (area == NULL) {
		 
		return c_red;
	}
	RGB* pixels = (RGB*) area->pixels;
	
	return *(pixels + y*area->w + x);
}


RGB graph_get_pixel(int x, int y) {
	if (sshot == NULL) return c_black;
	RGB* pixels = (RGB*) sshot->pixels;
	
	return *(pixels + y*graph_get_width() + x);
}

int rgb_red(RGB rgb) {
	return rgb & 0xff;
}

int rgb_green(RGB rgb) {
	return (rgb >> 8) & 0xff;
}

int rgb_blue(RGB rgb) {
	return (rgb >> 16) & 0xff;
}

bool graph_save_screen(const char *path) {
	SDL_Surface *sshot = SDL_CreateRGBSurface(0, graph_get_width(), graph_get_height(), 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(screen, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
	SDL_SaveBMP(sshot, path);
	SDL_FreeSurface(sshot);
	return true;
}

RGB graph_rgb(int r, int g, int b) {
	// gfx documentation is wrong
	// this is done according to the source code
	// ATTENTION! - This just works in little-endian arquitectures
	unsigned int rgb;
	byte *prb= (byte*) &rgb;
	prb[0] = (byte) r; prb[1] =  (byte) g; prb[2] = (byte) b; prb[3] = 255; // last byte  is alpha
	return rgb;
}


RGB graph_transparent() {
	return 0;
}

void graph_pixel(short x, short y, RGB color) {
	pixelColor(screen, x, y, color);
	if (sshot != NULL)  {
		RGB* pixels = (RGB*) sshot->pixels;
	    *(pixels + y*graph_get_width() + x) = color;
	}
}



void  graph_exit2(const char *name) {

   __done = true;
   
   if (ui_thread != pthread_self()) {
   
		SDL_Event event;
		SDL_memset(&event, 0, sizeof(event));
		event.user.type = _END_LOOP;
		event.user.code = END_SESSION_EVENT;
		event.user.data1 = (char*) name;
		event.user.data2 = NULL;

		event.type = _END_LOOP;

		SDL_PushEvent(&event);
    }
}

void  graph_exit() {
	graph_exit2(NULL);
}


void graph_line(short x1, short y1, short x2, short y2, RGB color) {
	lineColor(screen, x1, y1, x2, y2, color);
}

 
void graph_rect(short x0, short y0, short w, short h, RGB color, bool toFill) {
	// for some reason, filled rectangles have one more pixel in width 
	// compared to the  dimensions of a non filled rect. Adjust it here
	int x1 = x0+w, y1=y0+h;
    if (w < 0) x1++; else x1--;
    if (h < 0) y1++; else y1--;
	if (toFill)
		boxColor(screen, x0, y0, x1, y1, (Uint32) color);
	else
		rectangleColor(screen, x0, y0, x1, y1, (Uint32) color);
}

void graph_refresh() {
	SDL_RenderPresent(screen);
}


void  graph_circle(short x0, short y0, short radius, RGB color, bool toFill) { 
	if (toFill)
		filledCircleColor(screen, x0, y0, radius, (Uint32) color);
	else
		circleColor(screen, x0, y0, radius, (Uint32) color);
}
                
//int filledPieColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad,Sint16 start, Sint16 end, Uint32 color);
void graph_pie(short cx, short cy, short radius, short sangle, short eangle, RGB color, bool toFill) {
    if (toFill)
        filledPieColor(screen, cx, cy, radius, sangle, eangle, color);
    else
        pieColor(screen, cx, cy, radius, sangle, eangle, color);
}


void  graph_ellipse(short x0, short y0, short xr, short yr, RGB color, bool toFill) { 
	if (toFill)
		filledEllipseColor(screen, x0, y0, xr, yr, (Uint32) color);
	else
		ellipseColor(screen, x0, y0, xr, yr, (Uint32) color);
}


 
void graph_round_rect(short x0, short y0, short w, short h, RGB color, bool toFill) {
	int x1 = x0+w-1, y1=y0+h-1;
	if (toFill)
		roundedBoxColor(screen, x0, y0,x1, y1, 10, (Uint32) color);
	else
		roundedRectangleColor(screen, x0, y0, x1, y1, 10, (Uint32) color);
}

int graph_init2(const char title[], int width, int height) {
	SDL_Rect clip;
	
	 	
	if (internal_init(title, true, width, height) < 0)
		 return -1;
 
	 
	clip.x = 0;
	clip.y = 0;
	clip.w = width;
	clip.h = height;	
	SDL_RenderSetViewport(screen, &clip);
#ifdef DEBUG_START
	printf("start server!\n");
#endif

	start_server();
	
#ifdef DEBUG_START	
	printf("server started!\n");
#endif
	 
	return 0;
}

int graph_init() {
	return graph_init2("PG Graphics", GRAPH_WIDTH, GRAPH_HEIGHT);
}

int graph_get_width() {
	return window_width;
}

int graph_get_height() {
	return window_height;
}

