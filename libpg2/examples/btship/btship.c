

//should be the first include 
#include "pg/pglib.h"


#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
 
#include "btship.h"
#include "board.h"
#include "strutils.h"



// DEBUG

// #define DEBUG_START
// #define DEBUG_END
// #define DEBUG_GAME
// #define DEBUG_MSG



// globals


Clock mclock;
int clock_tick_count;

MsgView status_msg;


battleship_t battle;

char username[MAX_NAME_SIZE];
char opponent_name[MAX_NAME_SIZE];
char game_name[MAX_NAME_SIZE];

bool demo_mode = false;
session_t game_session;
state_t state;
bool send_result;
int turn; // MY_TURN or OPPON_TURN

 

// for random auto-play
int random_auto_play[NSQUARES*NSQUARES];
int remaining_parts;


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


void do_play(char *game, int x, int y) {
	char args[256];
	char letter = x + 'A';
	sprintf(args, "%s %c %d\n", SHOT, letter, y+1);
	
	srv_play(game_session, args);
}


void do_send_result(char *game, int x, int y, int target) {
	char args[256];
	char letter = x + 'A';
	sprintf(args, "%s %c %d %d\n", RESULT, letter, y+1, target);
	send_result = true;
	srv_send_result(game_session, args);
}

/**
 * does our play
 */
bool play(int x, int y) {
	
	if (turn != MY_TURN) return false;
	Point p = place_selected(&battle.oppon_board, x, y);
	
	if (p.x == -1) return false;
	draw_place_try(&battle.oppon_board, p.x, p.y);
	battle.last_play = p;
	
	turn = OPPON_TURN;
	show_curr_player();
	do_play(game_name, p.x, p.y);

	return true;
}



void  random_auto_play_init() {
	for(int i=0; i < NSQUARES*NSQUARES; ++i)
		random_auto_play[i] =i;
	remaining_parts = NSQUARES*NSQUARES;
}

void play_auto() {
	if (turn != MY_TURN) return;
	int random_pos = rand_range(1, remaining_parts)-1;
	int pp = random_auto_play[random_pos];
	random_auto_play[random_pos] = random_auto_play[--remaining_parts];
	
	Point p;
	p.y = pp / NSQUARES;
	p.x = pp % NSQUARES;
	
	draw_place_try(&battle.oppon_board, p.x, p.y);
	battle.last_play = p;
	
	do_play(game_name, p.x, p.y);
	turn = OPPON_TURN;
	show_curr_player();
	return;
}	


/**
 * handlers
 */
 
 
//
// auxilary event handlers functions
//

void process_opponent_shot(int x, int y) {	
	 int val = val_place(&battle.my_board, x, y);
	
	 if (val > 0) {
		 battle.total_injuries++;
	 }
	 shot_place(&battle.my_board, x,y);
	 do_send_result(game_name, x, y, val);
	 
	 //turn = MY_TURN;
	 show_curr_player();
	
	 if (battle.total_injuries == battle.total_parts) {
#ifdef DEBUG_END
		 printf("%s loose!\n", username);
#endif
		state = GameOver;
		show_loose_message();
		srv_close_session(game_session);
	 }
	
}

void process_play_result(int x, int y, int target) {
	// play format:  {A-J}' '{1-10} {0-5}
	 
	if (x != battle.last_play.x || y != battle.last_play.y) {
		error(BAD_RESULT_MSG); return;
	}
	
	if (target >= 0) {
		if ( val_place(&battle.oppon_board, x, y) == 0 ) {
			fill_place(&battle.oppon_board, x, y, target);
			draw_place(&battle.oppon_board, x, y);
			if (target > 0 )   {
				++battle.total_hits;
				
				if (battle.total_hits == battle.total_parts) {
#ifdef DEBUG_END
					printf("%s win!\n", username);
#endif
					
					show_victory_message();
					state = GameOver;
					srv_close_session(game_session);
					return;
				}
			}
		}

	}
	
}

bool process_creation_response(const char *resp) {
	// get args
	int rank; 



	int start = str_next_word(resp, 0, opponent_name, MAX_NAME_SIZE);
	if (start != -1) start = str_next_int(resp, start, &rank);
	
	if (start == -1) { error(BAD_CREATION_MSG); return false;  }
	if (rank == 1) {
#ifdef DEBUG_START
		printf("%s start the game\n", username);
#endif
		turn = MY_TURN;
	}
	else {
#ifdef DEBUG_START
		printf("The opponent start the game\n");
#endif
		turn = OPPON_TURN;
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
	char letter = '.';
	int num = -1;
	
	start = str_next(msg, start, msg_type, MAX_RESP_TYPE_SIZE, Letters);
	if (start != -1) start = str_next_char(msg, start, &letter);
	if (start != -1) start = str_next_int(msg, start, &num);
	
	if (start == -1 ) { 
		error("Invalid message received!\n"); return;
	}
 
	if (strcmp(SHOT, msg_type) == 0) 	
		process_opponent_shot(letter -'A', num-1);
	else if (strcmp(RESULT, msg_type)== 0) {
		int target;
		if ((start = str_next_int(msg, start, &target)) == -1) {
			error("Invalid result message!\n"); return;
		}
		process_play_result(letter -'A', num-1, target);
	}
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
				if (send_result) {
					send_result=false;
					turn = MY_TURN;
					 show_curr_player();
				 }
				break;	
		case GameOver:
				break;
		default:
			break;
	}
						
}


 
void timer_handler() {
	if (state >= GameOver) return;
	
	if (turn == MY_TURN && state == InGame) {
			if (demo_mode) {
				play_auto();
		 
			}
	}
	if (++clock_tick_count == SEC_TICKS) {
		clk_tick(&mclock);
		clk_show(&mclock);
		clock_tick_count =0;
		
	}
}

void mouse_handler(MouseEvent me) {
	if (me.type == MOUSE_BUTTON_EVENT && 
		me.state == BUTTON_PRESSED) {
			
		if (me.button == BUTTON_LEFT) {
#ifdef DEBUG_GAME
		printf("do play at %d, %d, turn=%d, state=%d\n", me.x, me.y, turn, state);
#endif
			if (me.button == BUTTON_LEFT && turn == MY_TURN && state == InGame) {
				play(me.x, me.y);
			}
		}
		else {
			graph_save_screen("screen.jpg");
		}
		 
	}
 
}
 


void prepare_board() {
	create_battleship(&battle, MY_BOARD_X, MY_BOARD_Y, MY_BOARD_X+300, MY_BOARD_Y);
	
	// draw background
	graph_rect(0,0, WINDOW_WIDTH, WINDOW_HEIGHT,  BACK_COLOR, true);
	
	populate_battleship(&battle);
	draw_board(&battle.my_board, true);
	draw_board(&battle.oppon_board, false);
	
	
	mv_create(&status_msg, STATUS_MSG_X, STATUS_MSG_Y, STATUS_MSG_SIZE, STATUS_MSG_FONT,MSG_TC, MSG_BC);
	mv_show_text(&status_msg, "Wait Opponent...", ALIGN_LEFT);
	
	clk_create(&mclock, CLOCK_X, CLOCK_Y, MEDIUM_CLOCK, c_green, c_dgray);

	clk_show(&mclock);
	state = Start;
	 
}
	
	
int main(int argc, char *argv[]) {
	
	char server_ip[MAX_NAME_SIZE];
	if (get_args(username, server_ip, game_name) != 3) return 1;
	

	random_auto_play_init();
	srand(time(NULL) + getpid());

#ifdef DEBUG_START
	printf("battleship initiated for %s\n", username);	
#endif
	graph_init2(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	graph_regist_mouse_handler(mouse_handler);
	graph_regist_timer_handler(timer_handler, 50);
	
	prepare_board(); 
	 
	game_session = srv_connect(server_ip, username, on_response, on_msg);
#ifdef DEBUG_START
	printf("battleship started for %s\n", username);
#endif
	graph_start();
#ifdef DEBUG_END
	printf("battleship terminated for %s!\n" , username);
#endif
	return 0;
}
