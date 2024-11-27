/*------------------------------------------
 O jogo do galo
 
 Jorge Martins, 2020
 
 -----------------------------------------*/
 
 
#include "ttt_board.h"


#define WINDOW_WIDTH (BOARD_WIDTH + 2*BOARD_MARGIN)
#define WINDOW_HEIGHT (BOARD_HEIGHT + 2*BOARD_MARGIN+ STATUS_MSG_HEIGHT)


// dimensões do tabuleiro

#define BOARD_X BOARD_MARGIN
#define BOARD_Y BOARD_MARGIN
#define BOARD_WIDTH 500
#define BOARD_HEIGHT 500
#define BOARD_MARGIN 20
#define BOARD_CELL_SIDE (BOARD_WIDTH/ SIDE)

#define BOARD_GRID_WEIGHT 10

// cor de fundo do tabuleiro
#define BOARD_COLOR graph_rgb(0, 220, 255)
#define BOARD_GRID_COLOR c_blue
#define CROSS_COLOR	c_red
#define BALL_COLOR  c_orange


// configuração do status msg
#define STATUS_MSG_TC c_orange
#define STATUS_MSG_BC graph_rgb(0, 0, 100)
#define STATUS_MSG_FONT	MEDIUM_FONT
#define STATUS_MSG_HEIGHT graph_font_size(STATUS_MSG_FONT).height
#define STATUS_MSG_CHARS (WINDOW_WIDTH/graph_font_size(STATUS_MSG_FONT).width)

#define STATUS_MSG_X 0
#define STATUS_MSG_Y (WINDOW_HEIGHT - STATUS_MSG_HEIGHT - 10)


// configuração do relógio

#define TIMER_PERIOD 1000
#define CLOCK_X (WINDOW_WIDTH/2 - 45)
#define CLOCK_Y	5
#define CLOCK_CLR c_orange
#define CLOCK_BACK_COLOR BOARD_COLOR
#define CLOCK_FONT LARGE_FONT



#define MY_TURN		0
#define OPPON_TURN	1

 

void ttt_draw_board();
	 
	
void draw_cross(int x, int y);
	 

void draw_ball(int x, int y);

void draw_piece(int x, int y, int piece);
bool screen_to_board(int mx, int my, Point *p);


