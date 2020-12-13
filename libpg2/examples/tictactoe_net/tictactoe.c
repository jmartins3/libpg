/*------------------------------------------
 O jogo do galo
 
 Jorge Martins, 2014
 
 -----------------------------------------*/
 
 
#include <stdio.h>
#include <stdbool.h>
#include <math.h>


#include "pg/pglib.h"
#include "strutils.h"

#include "tictactoe.h" 

#define GAME_TYPE	"tictactoe"

// #define DEBUG_MSG


	
#define WINDOW_TITLE "Tic-Tac-Toe"


#define TIMER_PERIOD  50

#define SEC_TICKS (1000 / TIMER_PERIOD)

#define CLOCK_X WINDOW_WIDTH/2 -20
#define CLOCK_Y 10

// States
typedef enum state { Start, CreateGame, InGame, GameOver, SessionClosed, Error } state_t;

 


session_t game_session;					// the game session

state_t state;							// the game state

char username[MAX_NAME_SIZE];			// current user name
char opponent_name[MAX_NAME_SIZE];		// the opponent name
char game_name[MAX_NAME_SIZE];			// the game name


MsgView status_msg;						// status messagebox 
Clock mclock;							// the game clock

TTT_Board theBoard;
 
 
int my_piece;							// our piece (BALL or CROSS)
int other_piece;
int turn;								// active player

//
// auxiliary functions
//


void show_victory_message() {
	mv_show_text(&status_msg, "You win!", ALIGN_CENTER);
}

void show_loose_message() {
	mv_show_text(&status_msg, "You loose!", ALIGN_CENTER);
}


void show_draw_message() {
	mv_show_text(&status_msg, "draw!", ALIGN_CENTER);
}


void error(const char *msg) {
	printf("error: %s\n", msg);
	state = Error;
}


void show_curr_player() {
	char msg[128];
	
	 
	if (turn == MY_TURN) 
		sprintf(msg, "player %-15s", username);
	else
		sprintf(msg, "player %-15s", opponent_name);
	
	mv_show_text(&status_msg, msg, ALIGN_LEFT); 
}


// game messages


void send_play(int x, int y) {
	char args[32];
	sprintf(args, "%s %d %d\n", PLAY_MSG, x, y);
	
	srv_play(game_session, args);
}

void process_play(int res, int piece) {
	if (res == RESULT_WIN) {
		if (piece == my_piece) show_victory_message();
		else show_loose_message();
		srv_close_session(game_session);
		state = GameOver;
	
	}
	else if (res == RESULT_DRAW) {	
		show_draw_message();
		state = GameOver;
		 
	}
	else {
		turn = (piece == my_piece) ? OPPON_TURN : MY_TURN;
		show_curr_player();
	}
}

void process_opponent_shot(int x, int y) {	
	int res = ttt_play(&theBoard, x, y, other_piece);
	process_play(res, other_piece);
}


bool process_creation_response(const char *resp) {
	// get args
	int rank; 

 

	int start = str_next_word(resp, 0, opponent_name, MAX_NAME_SIZE);
	if (start != -1) start = str_next_int(resp, start, &rank);
	
	if (start == -1) { error(BAD_CREATION_MSG); return false;  }
	if (rank == 1) {
		 
		turn = MY_TURN;
		my_piece = CROSS;
		other_piece = BALL;
	}
	else {
		 
		turn = OPPON_TURN;
		my_piece = BALL;
		other_piece = CROSS;
	}
	state = InGame;
	show_curr_player();
	return true;
}


void on_msg(const char sender[], const char msg[]) {
	if (state != InGame || turn != OPPON_TURN) {
		error(BAD_STATE_MSG); return;
	}
	
	char msg_type[MAX_RESP_TYPE_SIZE] = {0};
	
	int start = 0;
	int x, y;
	
	start = str_next(msg, start, msg_type, MAX_RESP_TYPE_SIZE, All);
	if (start != -1) start = str_next_int(msg, start, &x);
	if (start != -1) start = str_next_int(msg, start, &y);
	
	if (start == -1 ) return;
	
	if (strcmp(PLAY_MSG, msg_type) == 0) 	
		process_opponent_shot(x, y);
}


void on_response(int status, const char response[]) {
	if (status != STATUS_OK) {
		error(response);
	 
		mv_show_text(&status_msg, response, ALIGN_LEFT);
		srv_close_session(game_session);
		return;
		
	}
	switch(state) {
		case Start: // connected, now the game creation
				state = CreateGame;
				srv_new_game(game_session, GAME_TYPE, game_name);
				break;
		case CreateGame:
				if (process_creation_response(response)) {
					state = InGame;
					show_curr_player();
				}
				else
					state = Error;
				break;
		case InGame:
				break;	
		case GameOver:
				state = SessionClosed;
				break;
		default:
			break;
	}
						
}

bool screen_to_board(int mx, int my, Point *p) {
	int third_size_x = BOARD_WIDTH/3+1;
	int third_size_y = BOARD_HEIGHT/3+1;


	int x = (mx - BOARD_X);
	if ( x < 0) return false;
	x = x /third_size_x;

	int y = (my -BOARD_Y);
	if ( y < 0) return false;
	y = y /third_size_y;

	if (y < 0 || x < 0 || y > 2 || x > 2 ) {
		p->x = p->y = -1;
		return false;
	}
	p-> x  = x;
	p-> y  = y;
	return true;
}


void mouse_handler(MouseEvent me) {
	
	if (me.type == MOUSE_BUTTON_EVENT && me.state == BUTTON_CLICK) {
		if (state == SessionClosed) {
			graph_exit();
			return;
		}
		else {
			Point bp;
			 
			
			if (state != InGame ||  turn != MY_TURN || 
			    !screen_to_board(me.x, me.y, &bp)) {
				return;  // cannot play
			}
			 
			int res = ttt_play(&theBoard, bp.x, bp.y, my_piece);
			if (res != RESULT_BAD) {
				send_play(bp.x, bp.y);
				process_play(res, my_piece);
			}
		}
		
	}
}


void timer_handler() {
	if (state == GameOver) return;
	clk_tick(&mclock);
	clk_show(&mclock);
}


void prepare_game() {
 	// draw background
	graph_rect(0,0, WINDOW_WIDTH, WINDOW_HEIGHT,  BACK_COLOR, true);
	
	ttt_create_board(&theBoard);
	ttt_draw_board(BACK_COLOR);
		
	 	
	mv_create(&status_msg, STATUS_MSG_X, STATUS_MSG_Y, STATUS_MSG_SIZE, STATUS_MSG_FONT,MSG_TC, MSG_BC);
	mv_show_text(&status_msg, "Wait Opponent...", ALIGN_LEFT);
	
	clk_create(&mclock, CLOCK_X, CLOCK_Y, MEDIUM_CLOCK, c_green, c_dgray);
	clk_show(&mclock);
	
	state = Start;
	 
}
	

int main() {
	
	char server_ip[MAX_NAME_SIZE];
	
	 
	if (get_args(username, server_ip, game_name) != 3) {
		printf("usage: tictactoe <username> <server_addr> <game_name>\n");
		return 1;
	}
	
	 
	graph_init2(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	graph_regist_mouse_handler(mouse_handler);
	graph_regist_timer_handler(timer_handler, 1000);
	
	prepare_game(); 
	
	 
	game_session = srv_connect(server_ip, username, on_response, on_msg);
	 
	graph_start();
 
	return 0;
}


 
