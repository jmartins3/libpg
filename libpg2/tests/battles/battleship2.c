#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>

#include <stdlib.h>
#include <stdbool.h>


#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 350


#include "pg/graphics.h"
#include "components.h"
#include "battleship.h"
#include "board.h"
#include "strutils.h"


#define GAME_NAME "battle_john_jmartins"

#define MY_BOARD_X 50
#define MY_BOARD_Y 90


#define MY_TURN		0
#define OPPON_TURN	1

#define MSG_PLAYER_X	20
#define MSG_PLAYER_Y	35
 


#define MSG_X 200
#define MSG_Y 300
#define MSG_BC c_dgray
#define MSG_TC c_orange


MsgView msg;

bool demo_mode = false;

battleship_t battle;

typedef enum state { Start, CreateGame,  InGame, Done, Error } state_t;

char *username;
char opponent_name[64];

session_t game_session;
state_t state;

int turn; // MY_TURN or OPPON_TURN

typedef enum result { WATER, HIT, BAD } result_t;

void play_auto();


void do_play(char *game, int x, int y) {
	char args[256];
	char letter = x + 'A';
	sprintf(args, "battleship %s\nshot %c %d\n\n", 
				game, letter, y+1);
	gs_request(game_session, PLAY, args);
}


void do_response(char *game, int x, int y, int target) {
	char args[256];
	char letter = x + 'A';
	sprintf(args, "battleship %s\nresult %c %d %d\n\n", 
				game, letter, y+1, target);
	gs_request(game_session, PLAY, args);
}


void error(const char *msg) {
	printf("error: %s\n", msg);
	state = Error;
}


void show_curr_player() {
	char msg[128];
	
	if (state != InGame) 
		sprintf(msg, "Waiting for opponent...");
	else if (turn == MY_TURN) 
		sprintf(msg, "PLAYER ONE - %s    ", username);
	else
		sprintf(msg, "PLAYER TWO - %s    ", opponent_name);
	 
	graph_text(MSG_PLAYER_X, MSG_PLAYER_Y, c_gray ,  msg, LARGE_FONT);
	 
}


bool play(int x, int y) {
	
	if (turn != MY_TURN) return false;
	Point p = place_selected(&battle.oppon_board, x, y);
	
	printf("play at board coords %c,%d\n", 'A' + p.x, p.y+1);
	if (p.x == -1) return false;
	draw_place_try(&battle.oppon_board, p.x, p.y);
	battle.last_play = p;
	do_play(GAME_NAME, p.x, p.y);
	
	
	turn = OPPON_TURN;
	show_curr_player();
	return true;
}

bool my_board_visible = false;
bool oppon_board_visible = false;



void toggle_board(int x, int y) {
	Point p = place_selected(&battle.my_board, x, y);
	if (p.x != -1) {
		if (my_board_visible) {
			hide_board(&battle.my_board);
			my_board_visible = false;
		}
		else {
			draw_board(&battle.my_board, true);
			my_board_visible = true;
		}
	}
}
	



void show_victory_message() {
	mv_show_text(&msg, "I win!", ALIGN_CENTER);
}

void show_loose_message() {
	mv_show_text(&msg, "I loose!", ALIGN_CENTER);
}


void process_opponent_shot(int x, int y) {	
	 int val = val_place(&battle.my_board, x, y);
	 do_response(GAME_NAME, x, y, val);
	 if (val > 0) {
		 battle.total_injuries++;
		 shot_place(&battle.my_board, x, y);
		
		 if (battle.total_injuries == battle.total_parts) {
			state = Done;
			printf("I Loose\n");
		
			show_loose_message();
			return;
		 }
	 }
	 turn = MY_TURN;
	 show_curr_player();
}

void process_opponent_result(int x, int y, int target) {
	// play format:  {A-J}' '{1-10} {0-5}
	 
	if (x != battle.last_play.x || y != battle.last_play.y) {
	 error("Inconsistent opponent result!\n"); return;
	}
	printf("Process opponent response at %c-%d\n", 'A'+x, y+1);
	if (target >= 0) {
		if ( val_place(&battle.oppon_board, x, y) == 0 ) {
			fill_place(&battle.oppon_board, x, y, target);
			draw_place(&battle.oppon_board, x, y);
			if (target > 0 )   {
				++battle.total_hits;
				printf("hit at %c-%d! total_hits=%d, total_parts=%d\n", 
						battle.last_play.x+'A', battle.last_play.y +1,
						battle.total_hits, battle.total_parts);
				// fprintf(stderr, "press enter to continue..."); getchar();
				if (battle.total_hits == battle.total_parts) {
					state = Done;
					printf("I Win\n");
					show_victory_message();
					return;
				}
			}
		}

	}
	
}


void play_auto() {
	if (turn != MY_TURN) return;
	Point p;
	do {
		int x = rand_range(0, 9);
		int y = rand_range(0,9);
		p.x = x;
		p.y = y;
	}
	while(val_place(&battle.oppon_board, p.x, p.y) > 0);
	printf("play at board coords %c,%d\n", 'A' + p.x, p.y+1);
	 
	draw_place_try(&battle.oppon_board, p.x, p.y);
	battle.last_play = p;
	do_play(GAME_NAME, p.x, p.y);
	
	turn = OPPON_TURN;
	show_curr_player();
	return;
}	
	


bool process_creation_response(const char *resp) {
	int rank; 
	
	int res = sscanf(resp, "%s %d", opponent_name, &rank);
	if (res != 2) return false;
	if (rank == 1) {
		printf("I start the game");
		turn = MY_TURN;
	}
	else {
		printf("The opponent start the game");
		turn = OPPON_TURN;
	}
	
	return true;
}



#define MAX_RESP_TYPE_SIZE 10



void on_msg(const char sender[], const char msg[]) {
	printf("msg send from group joiner %s: %s\n", sender, msg);

	if (state != InGame || turn != OPPON_TURN) {
		error("Bad state"); return;
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
	
	printf("msg received: type='%s', letter= '%c', num=%d\n", msg_type, letter, num);
	if (strcmp("shot", msg_type) == 0) 	
		process_opponent_shot(letter -'A', num-1);
	else if (strcmp("result", msg_type)== 0) {
		int target;
		if ((start = str_next_int(msg, start, &target)) == -1) {
			error("Invalid result message!\n"); return;
		}
		process_opponent_result(letter -'A', num-1, target);
	}
}


void on_response(int status, const char response[]) {
	if (status < STATUS_OK) {
		printf("error: %s\n", response);
		state = Error;
		return;
	}
	switch(state) {
		case Start:
				printf("Now create the game\n");
				state = CreateGame;
				gs_new_game(game_session, GAME_NAME);
				break;
		case CreateGame:
				printf("game started!\n");
				if (process_creation_response(response)) {
					state = InGame;
					show_curr_player();
				}
				else
					state = Error;
				break;
		default:
			break;
	}
						
}


void timer_handler() {
	if (turn == MY_TURN && state == InGame)
		if (demo_mode) play_auto();
}


void mouse_handler(MouseEvent me) {
	if (me.type == MOUSE_BUTTON_EVENT && 
		me.state == BUTTON_PRESSED) {
		printf("do play at %d, %d, turn=%d, state=%d\n", me.x, me.y, turn, state);
		if (me.button == BUTTON_LEFT && turn == MY_TURN && state == InGame) {
			
			play(me.x, me.y);
		}
	 
	}
}

	
int main(int argc, char *argv[]) {
	srand(time(NULL));
	graph_init2(WINDOW_WIDTH, WINDOW_HEIGHT);
	
	create_battleship(&battle, MY_BOARD_X, MY_BOARD_Y, MY_BOARD_X+300, MY_BOARD_Y);
	
	populate_battleship(&battle);
	draw_board(&battle.my_board, true);
	draw_board(&battle.oppon_board, false);
	msg = mv_create(MSG_X, MSG_Y, 20, MEDIUM_FONT,MSG_TC, MSG_BC);
	mv_set_margins(&msg, 10, 20);
	
	char *server_ip ;
	if (argc == 3) {
		username = argv[1];
		server_ip = argv[2];
	}
	else {
		server_ip = getenv("REG_SERVER_IP");
		username = getenv("GAME_USER");
	}

	graph_regist_mouse_handler(mouse_handler);
	graph_regist_timer_handler(timer_handler, 500);
	
	state = Start;
	show_curr_player();
	game_session = gs_connect(server_ip, username, on_response, on_msg);
	printf("\nStart!\n\n");
	graph_start();
	return 0;
}
