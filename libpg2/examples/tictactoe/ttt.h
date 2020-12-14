/*------------------------------------------
 O jogo do galo
 
 Jorge Martins, 2020
 
 -----------------------------------------*/
 
 
 

#define WINDOW_WIDTH 				500
#define WINDOW_HEIGHT 				440

 

#define BACK_COLOR graph_rgb(0, 220, 240)


#define BOARD_X	0
#define BOARD_Y	0

#define GRID_WEIGHT	10
#define SQUARE_BORDER 30
#define SQUARE_WEIGHT	12
#define BOARD_WIDTH	500
#define BOARD_HEIGHT 440

#define MSG_BC c_dgray
#define MSG_TC c_orange


#define STATUS_MSG_FONT MEDIUM_FONT
#define STATUS_MSG_X	0
#define STATUS_MSG_Y	(WINDOW_HEIGHT-graph_font_size(STATUS_MSG_FONT).height-10)
#define STATUS_MSG_SIZE ((WINDOW_WIDTH)/ graph_font_size(STATUS_MSG_FONT).width)

#define MY_TURN		0
#define OPPON_TURN	1

 

void ttt_draw_board(RGB back_color);
	 
	
void draw_cross(int x, int y);
	 

void draw_circle(int x, int y);


bool screen_to_board(int mx, int my, Point *p);


