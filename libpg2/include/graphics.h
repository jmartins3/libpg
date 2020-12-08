#ifndef _GRAPHICS_H

#define GRAPHICS_H


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef __cplusplus
#include 	<stdbool.h>
#endif

#include <uv.h>
#include "events.h"
#include "socket_events.h"
#include "audio.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>



extern volatile int __done;

typedef Uint32 RGB;

typedef unsigned char byte;

// tipo para representar uma posição no écrâ
typedef struct {
	int x;
	int y;
} Point;

// tipo para representar uma dimensão 2D
typedef struct {
	int width, height;
} Size;

// tipo que representa uma área rectangular
typedef struct {
	Point p;
	Size sz;
} Rect;

typedef SDL_Surface *ScrArea;

#ifdef LIBPG_IMPL
void timebase_regist();
extern SDL_Renderer *screen;
#endif





#ifdef __cplusplus 

extern "C" {
	
#endif


/**
 *  returns an rgb color from components
 */
RGB graph_rgb(int r, int g, int b);

/**
 *  returns the "transparent" color
 */
RGB graph_transparent();

/**
 *  initializes the graphics. Must be called first	 
 */
int graph_init();

/**
 * initializes the graphics, specifing dimensions and a title for the graphic window
 * Must be called first	 
 */
int graph_init2(const char title[], int width, int height);

/**
 *  suspend execution for x miliseconds (deprecated)
 */
void graph_delay(int ms);

/**
 *  draw a pixel at position with specified color
 */
void graph_pixel(short x, short y, RGB color);
               
/**
 *  draw a line at position with specified color
 */
void graph_line(short x1, short y1, short x2, short y2, RGB color);
	 
/**
 *  draw a circle at position with specified color, filled or not
 */
void  graph_circle(short x0, short y0, short radius, RGB color, bool toFill);
	            
/**
 *  draw a rect at position with specified color, filled or not
 */
void graph_rect(short x0, short y0, short w, short h, RGB color, bool toFill);
	 
	 
/**
 *  draw a ellipse at position with specified color, filled or not
 */
void  graph_ellipse(short x0, short y0, short xr, short yr, RGB color, bool toFill);


/**
 *  draw a round rectangle at position with specified color, filled or not
 */
void graph_round_rect(short x0, short y0, short w, short h, RGB color, bool toFill);

/**
 *  draw a triangle at position (x0,y0) with specified color, filled or not
 */
int graph_triangle2(int x0, int y0, int a, int b, int c, RGB color, bool toFill);

/**
 *  draw a triangle given his points (x0,y0), (x1,y1), (x2,y2)
 * with specified color, filled or not
 */
void graph_triangle(int x0, int y0, int x1, int y1, int x2, int y2, RGB color, bool toFill );

/**
 *  start the event loop
 */
void graph_start();

/**
 *  stop the event loop
 */
void  graph_exit();	

/**
 *  stop the event loop with a name (for debug purposes)
 */
void  graph_exit2(const char* name);	
	 
/**
 *  refresh graphics windows. Must be called to show done operations result
 */
void graph_refresh();

/**
 *  get the graph window width
 */
int graph_get_width();
	 
/**
 *  get the graph_window height
 */
int graph_get_height();
	 

// Text Functions

//font types for text write
#define SMALL_FONT 	1
#define MEDIUM_FONT	2
#define LARGE_FONT	3

/**
 * Writes a string starting at point x,y with "color" as text color
 * and witth font size specified in fontsize (SMALL, MEDIUM, LARGE)
 */
void graph_text( short x, short y, RGB color, const char text[], int fontsize );

/**
 * Apresenta o texto passado em "text". Os parâmetros são:  
 * 		x,y é o ponto de inicio (canto inferior esquerdo da área do texto)
 * 		"fore_color" é a cor da letra e back_color é a cor de fundo. 
 * 		"text" é a string que se deseja escrever;
 * 		"font_type" indica a fonte a usar: SMALL, MEDIUM, LARGE
 */
void graph_text2( short x, short y, RGB fore_color, RGB back_color, const char text[], int font_type );

/**
 * Retorna uma estrutura Size com as dimensões da fonte 
 * de texto especificada.
 * Parâmetros:
 * 		"font_type": tipo da fonte (SMALL_FONT, MEDIUM_FONT, LARGE_FONT)
 * Retorna:
 * 		estrutura Size com as dimensões da fonte
 */
Size graph_font_size(int font_type);
	 

/**
 * Retorna uma estrutura Size com as dimensões ocupado pelo número
 * de caracteres na  dimensão de fonte indicados
 * Parâmetros:
 * 		"nchars": número de caracteres
 * 		"font": dimensão da fonte (SMALL_FONT, MEDIUM_FONT, LARGE_FONT)
 * Retorna:
 * 		estrutura Size com as dimensões do texto
 */
Size graph_chars_size(int nchars, int font);
	 
	
/**
 * Retorna uma estrutura Size com as dimensões ocupado pelo texto "text"
 * na  dimensão de fonte "font"
 * Parâmetros:
 * 		"text": string com o texto a avaliar
 * 		"font": dimensão da fonte (SMALL_FONT, MEDIUM_FONT, LARGE_FONT)
 * Retorna:
 * 		estrutura Size com as dimensões do texto
 */
Size graph_text_size(const char text[], int font);
	 

// Image functions	 

/**
 *  apresenta o conteúdo de um ficheiro imagem (ex: jpg) na janela gráfica
 *	Parâmetros:
 *  	"path" : nome do ficheiro imagem
 * 		"x", "y": coordenadas do canto superior esquerdo do "viewport"
 * 		"width", "height": dimensões do "viewport"
 */
bool graph_image(const char *path, int x, int y, int width, int height);

/**
 * captura o conteúdo da janela gráfica para um ficheiro
 */
bool graph_save_screen(const char *path);

void graph_start_capture();
void graph_end_capture();

/**
 *  Obtem o pixel na posição "x", "y" da janela gráfica
 *  Para chamar esta função (uma ou mais vezes) é necessária
 *  invocar as funções graph_start_capture e graph_end_capture imediatamente
 *  antes e depois da utilização, respectivamente.
 */
RGB  graph_get_pixel(int x, int y);


ScrArea graph_start_capture2(Rect r);
void graph_end_capture2(ScrArea area);	

/**
 *  Obtém o pixel na posição "x", "y" dentro da área da janela gráfica
 *  representada pela ScrArea "area".
 *  Para chamar esta função (uma ou mais vezes) é necessário
 *  invocar as funções graph_start_capture2 e graph_end_capture2 imediatamente
 *  antes e depois  da utilização, respectivamente.
 */ 
RGB graph_get_pixel2(ScrArea area, int x, int y);

/**
 * obter as componentes "red", "green" e "blue" de uma côr RGB
 */
int rgb_red(RGB rgb);
int rgb_green(RGB rgb);
int rgb_blue(RGB rgb);

	 
// Sound functions

/**
 * Toca o ficheiro audio (apenas formato raw) de nome "path"
 */
bool  sound_play(const char *path);

/**
 * Pausa a reprodução do som
 */
void sound_pause();

/**
 * Recomeça a reprodução do som
 */
void sound_resume();

/**
 * Para a reprodução do som
 */
void sound_stop();
	 
#ifdef __cplusplus 
}
#endif


// Cores pré-definidas

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
#define c_yellow		graph_rgb(255, 255, 0)
#define c_transparent	((RGB) 0)

#endif
