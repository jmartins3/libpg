/*------------------------------------------
 O jogo do galo
 
 Jorge Martins, 2014
 
 -----------------------------------------*/
 
 
#include <stdio.h>
#include <stdbool.h>
#include <math.h>


#include "pg/pglib.h"
#include "strutils.h"
#include "comm.h"
#include "tictactoe.h" 

#define DEBUGN

	
#define WINDOW_TITLE "Tic-Tac-Toe"


#define TIMER_PERIOD  50

#define SEC_TICKS (1000 / TIMER_PERIOD)

#define CLOCK_X WINDOW_WIDTH/2 -20
#define CLOCK_Y 10

// States
typedef enum state { Start, CreateGame, InGame, GameOver, Error } state_t;

 


session_t game_session;					// the game session

state_t state;							// the game state

char username[MAX_NAME_SIZE];			// current user name
char opponent_name[MAX_NAME_SIZE];		// the opponent name
char game_name[MAX_NAME_SIZE];			// the game name


MsgView status_msg;						// status messagebox 
Clock mclock;							// the game clock

TTT_Board theBoard;


int nplays;								// total plays
 
int my_piece;							// our piece (BALL or CROSS)
int other_piece;
int turn;								// active player

//
// auxiliary functions
//


void show_victory_message() {
	mv_show_text(&status_msg, "I win!", ALIGN_CENTER);
}

void show_loose_message() {
	mv_show_text(&status_msg, "I loose!", ALIGN_CENTER);
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


void do_play(int x, int y) {
	char args[32];
	sprintf(args, "%s %d %d\n", PLAY_MSG, x, y);
	
	bs_play(game_session, args);
}


void process_opponent_shot(int x, int y) {	
	ttt_play(x, y, other_piece);

	nplays++;
	if (winner(other_piece)) {
		printf("player with %s won!\n", other_piece == CROSS ? "cross" : "ball");
		state = GameOver;
	
	}
	else if (nplays == 9) {	
		printf("draw!\n");
		state = GameOver;
		 
	}
 	turn = MY_TURN;
		
}


bool process_creation_response(const char *resp) {
	// get args
	int rank; 

printf("creation_response: '%s'\n", resp);

	int start = str_next_word(resp, 0, opponent_name, MAX_NAME_SIZE);
	if (start != -1) start = str_next_int(resp, start, &rank);
	
	if (start == -1) { error(BAD_CREATION_MSG); return false;  }
	if (rank == 1) {
		printf("%s start the game\n", username);
		turn = MY_TURN;
		my_piece = CROSS;
		other_piece = BALL;
	}
	else {
		printf("The opponent start the game\n");
		turn = OPPON_TURN;
		my_piece = BALL;
		other_piece = CROSS;
	}
	state = InGame;
	show_curr_player();
	return true;
}


void on_msg(const char sender[], const char msg[]) {
#ifdef DEBUGN	
	printf("msg received from group joiner %s: %s\n", sender, msg);
#endif
	if (state != InGame || turn != OPPON_TURN) {
printf("state=%d, turn=%d\n", state, turn);
		error(BAD_STATE_MSG); return;
	}
	char msg_type[MAX_RESP_TYPE_SIZE] = {0};
	int start = 0;
	int x, y;
	
	start = str_next(msg, start, msg_type, MAX_RESP_TYPE_SIZE, All);
	if (start != -1) start = str_next_int(msg, start, &x);
	if (start != -1) start = str_next_int(msg, start, &y);
	
	if (start == -1 ) {
		 error("Invalid message received!\n"); return;
	}
#ifdef DEBUG
	printf("msg received: type='%s', x= '%c', y=%d\n", msg_type, x, y);
#endif
	if (strcmp(PLAY_MSG, msg_type) == 0) 	
		process_opponent_shot(x, y);
}


void on_response(int status, const char response[]) {
	if (status != STATUS_OK) {
		error(response);
		printf("error %d: %s\n", status, response);
		mv_show_text(&status_msg, response, ALIGN_LEFT);
		bs_close_session(game_session);
		return;
		
	}
	switch(state) {
		case Start: // connected, now the game creation
				state = CreateGame;
				bs_new_game(game_session, game_name);
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
				printf("end game!\n");
			
				break;
		default:
			break;
	}
						
}


void mouse_handler(MouseEvent me) {
	
	if (me.type == MOUSE_BUTTON_EVENT && me.state == BUTTON_CLICK) {
		if (state == GameOver) {
			graph_exit();
			return;
		}
		
		Point bp;
		if (!screen_to_board(me.x, me.y, &bp)) return;
		
		if (turn == MY_TURN && ttt_play(bp.x, bp.y, my_piece)) {
			do_play(bp.x, bp.y);
			nplays++;
			if (winner(my_piece)) {
				printf("player with %s won!\n", my_piece == CROSS ? "cross" : "ball");
				state = GameOver;
			
			}
			else if (nplays == 9) {	
				printf("draw!\n");
				state = GameOver;
				 
			}
			
			turn = OPPON_TURN;
		}
		
	}
}


void prepare_game() {
 	// draw background
	graph_rect(0,0, WINDOW_WIDTH, WINDOW_HEIGHT,  BACK_COLOR, true);
	
	ttt_draw_board();
		
	 	
	 
	mv_create(&status_msg, STATUS_MSG_X, STATUS_MSG_Y, STATUS_MSG_SIZE, STATUS_MSG_FONT,MSG_TC, MSG_BC);
	mv_show_text(&status_msg, "Wait Opponent...", ALIGN_LEFT);
	
	clk_create(&mclock, CLOCK_X, CLOCK_Y, MEDIUM_CLOCK, c_green, c_dgray);

	clk_show(&mclock);
	state = Start;
	 
}
	

int main() {
	
	char server_ip[MAX_NAME_SIZE];
	
	printf("enter tic-tac-toe...\n");
	if (get_args(username, server_ip, game_name) != 3) {
		printf("usage: tictactoe <username> <server_addr> <game_name>\n");
		return 1;
	}
	
	printf("tic_tac_toe initiated for %s\n", username);	
	graph_init2(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	graph_regist_mouse_handler(mouse_handler);
	
	prepare_game(); 
	
	printf("tic_tac_toe created for %s\n", username);	 
	game_session = bs_connect(server_ip, username, on_response, on_msg);
	printf("tic_tac_toe started for %s\n", username);
	graph_start();
	printf("tic_tac_toe terminated for %s!\n" , username);
	return 0;
}


 
