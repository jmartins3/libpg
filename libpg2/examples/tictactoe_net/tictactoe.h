/*------------------------------------------
 O jogo do galo
 
 Jorge Martins, 2014
 
 -----------------------------------------*/

#define SIDE						3



#define PLAY_MSG  "PLAY"

// Warning Messages

#define BAD_STATE_MSG 				"msg received in the wrong state"
#define BAD_CREATION_MSG			"bad creation message"

 

#define WINDOW_WIDTH 				700
#define WINDOW_HEIGHT 				550

#define MAX_RESP_TYPE_SIZE			16

#define BACK_COLOR graph_rgb(0, 220, 240)

/* O modelo */

#define BALL	1
#define CROSS	2

#define RESULT_DRAW		0
#define RESULT_WIN		2
#define RESULT_LOOSE	3
#define RESULT_OK		1
#define RESULT_BAD		-1

typedef struct {
	int board[SIDE][SIDE];
	int nplays;
} TTT_Board;

#define BOARD_X	90
#define BOARD_Y	60

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


#define min(x,y) ((x) < (y) ? (x) : (y))


void ttt_draw_board(RGB back_color);
	 
	
void draw_cross(int x, int y);
	 

void draw_circle(int x, int y);


bool screen_to_board(int mx, int my, Point *p);

int ttt_play(TTT_Board *board, int x, int y, int piece);

void ttt_create_board(TTT_Board *board);
