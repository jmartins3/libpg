 

#define GAME_TYPE "battleship"

#define WINDOW_TITLE "Battleship"

// Window size	
#define WINDOW_WIDTH 		600
#define WINDOW_HEIGHT 		350

// CLOCK e TIMER

#define TIMER_PERIOD  	50

#define SEC_TICKS 		(1000 / TIMER_PERIOD)

#define CLOCK_X 		WINDOW_WIDTH/2 -20
#define CLOCK_Y 		10


// Sound
#define BOMB_BLOW 		"./bomb.wav"


// game message names
#define SHOT 			"SHOT"
#define RESULT			"RESULT"

 

// Warning Messages

#define BAD_RESULT_MSG 		"bad opponent result"
#define BAD_SHOT_MSG		"bad opponent play"
#define BAD_STATE_MSG 		"msg received in the wrong state"
#define BAD_CREATION_MSG	"bad creation message"

#define MAX_RESP_TYPE_SIZE 	 20



// board configuration

#define BACK_COLOR graph_rgb(0, 170, 170)
#define MY_BOARD_X 50
#define MY_BOARD_Y 90
 
 

#define MSG_BC c_dgray
#define MSG_TC c_orange


#define STATUS_MSG_FONT MEDIUM_FONT
#define STATUS_MSG_X	0
#define STATUS_MSG_Y	(WINDOW_HEIGHT-graph_font_size(STATUS_MSG_FONT).height-10)
#define STATUS_MSG_SIZE ((WINDOW_WIDTH)/ graph_font_size(STATUS_MSG_FONT).width)

#define MY_TURN		0
#define OPPON_TURN	1



// States
typedef enum state { Start, CreateGame, InGame, GameOver, Error } state_t;
