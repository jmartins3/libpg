#pragma once

// 
// server codes

#define STATUS_OK			201



#define GRAPH_WIDTH 1024
#define GRAPH_HEIGHT 768
#define BOMB_BLOW "./bomb.wav"

// game message names
#define SHOT 	"SHOT"
#define RESULT	"RESULT"

// notification messsages



// Warning Messages

#define BAD_RESULT_MSG 				"bad opponent result"
#define BAD_SHOT_MSG				"bad opponent play"
#define BAD_STATE_MSG 				"msg received in the wrong state"
#define BAD_CREATION_MSG			"bad creation message"

#define MAX_RESP_TYPE_SIZE 	 20

// Window size	
#define WINDOW_WIDTH 		600
#define WINDOW_HEIGHT 		350



#define GAME_NAME "battle_john_jmartins"


// board configuration

#define BACK_COLOR graph_rgb(0, 170, 170)
#define MY_BOARD_X 50
#define MY_BOARD_Y 90

#define MSG_PLAYER_X	20
#define MSG_PLAYER_Y	35
 
#define MSG_X 			200
#define MSG_Y 			300
#define MSG_SIZE 		20
#define MSG_MARGIN_X 	10
#define MSG_MARGIN_Y	20

#define MSG_BC c_dgray
#define MSG_TC c_orange


#define STATUS_MSG_FONT MEDIUM_FONT
#define STATUS_MSG_X	0
#define STATUS_MSG_Y	(WINDOW_HEIGHT-graph_font_size(STATUS_MSG_FONT).height-10)
#define STATUS_MSG_SIZE ((WINDOW_WIDTH)/ graph_font_size(STATUS_MSG_FONT).width)

#define MY_TURN		0
#define OPPON_TURN	1



