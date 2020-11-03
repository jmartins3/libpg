#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>

#include <stdlib.h>
#include <stdbool.h>


#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 350
// 
// server codes
#define ERR_TOPIC_DUPLICATE 432
#define STATUS_OK			201

#include "pg/graphics.h"
#include "components.h"

#include "board.h"


#define GAME_NAME "battle_john_jmartins"

#define MY_BOARD_X 50
#define MY_BOARD_Y 90


#define MY_TURN		0
#define OPPON_TURN	1

#define MSG_PLAYER_X	20
#define MSG_PLAYER_Y	35
 
 
bool demo_mode = false;

battleship_t battle;

typedef enum state { Start, CreateGame, JoinGame, WaitPartner, InGame, Done, Error } state_t;

char *username;
char opponent_name[64];

session_t game_session;
state_t state;

int turn; // MY_TURN or OPPON_TURN

typedef enum result { WATER, HIT, BAD } result_t;

void play_auto();

 
void create_game(char *game, int nplayers) {
	char args[256];
	int port = session_get_msg_port(game_session);
	sprintf(args, "battleship %s %d\n%d\n\n",  game, nplayers, port);
	gs_request(game_session, CREATE_GAME, args);
}

void join_game(char *game) {
	char args[256];
	int port = session_get_msg_port(game_session);
	sprintf(args, "battleship %s\n%d\n\n",  game, port);
	gs_request(game_session, JOIN_GAME, args);
}

void do_play(char *game, int x, int y, int target) {
	char args[256];
	char letter = x + 'A';
	sprintf(args, "battleship %s\n%c %d %d\n\n", 
				game, letter, y+1, target);
	gs_request(game_session, PLAY, args);
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
	do_play(GAME_NAME, p.x, p.y, battle.last_target);
	
	
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
	
void mouse_handler(MouseEvent me) {
	if (me.type == MOUSE_BUTTON_EVENT && 
		me.state == BUTTON_PRESSED) {
		printf("do play at %d, %d, turn=%d, state=%d\n", me.x, me.y, turn, state);
		if (me.button == BUTTON_LEFT && turn == MY_TURN && state == InGame) {
			
			play(me.x, me.y);
		}
	 
	}
}

MsgView msg;
#define MSG_X 200
#define MSG_Y 300
#define MSG_BC c_dgray
#define MSG_TC c_orange


void show_victory_message() {
	mv_show_text(&msg, "I win!", ALIGN_CENTER);
}

void show_loose_message() {
	mv_show_text(&msg, "I loose!", ALIGN_CENTER);
}
		

void process_opponent_response(int x, int y, int target) {
	 // play format:  {A-J}' '{1-10} {0-5}
	 
	 printf("Process opponent response at %c-%d\n", 'A'+x, y+1);
	 if (target >= 0) {
		if ( val_place(&battle.oppon_board, battle.last_play.x, battle.last_play.y) == 0 ) {
			 
		
			fill_place(&battle.oppon_board, battle.last_play.x, battle.last_play.y, target);
			draw_place(&battle.oppon_board, battle.last_play.x, battle.last_play.y);
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
	 
	 int val = val_place(&battle.my_board, x, y);
	 battle.last_target = val;
	 if (val != EMPTY) {
		 battle.total_injuries++;
		 if (battle.total_injuries == battle.total_parts) {
			state = Done;
			printf("I Loose\n");
			do_play(GAME_NAME, 0, 0, val);
			show_loose_message();
			return;
		 }
	 }
	 turn = MY_TURN;
	 show_curr_player();
	
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
	do_play(GAME_NAME, p.x, p.y, battle.last_target);
	
	turn = OPPON_TURN;
	show_curr_player();
	return;
}	
	
void on_msg(const char sender[], const char msg[]) {
	 printf("msg send from group joiner %s: %s\n", sender, msg);
	 
	 if (state == WaitPartner) {
		 printf("game is active!\n");
		 state = InGame;
		 strcpy(opponent_name, sender);
		 turn = MY_TURN;
		 show_curr_player();
		 if (demo_mode) play_auto();
		 return;
	 }
	 if (state == InGame && turn == OPPON_TURN) {
		 char letter;
		 int  num;
		 int  target;
		 sscanf(msg, "%c%d%d", &letter, &num, &target);
		 process_opponent_response(letter-'A', num-1, target);
	 }
}

void get_opponent_from_response(const char *resp, char *opponent_name) {
	int i=0;
	while(resp[i] != '\n') ++i;
	++i;
	int j =0;
	while(resp[i] != '\n') opponent_name[j++] =resp[i++];
	opponent_name[j] = 0;
	printf("opponent_name=%s\n", opponent_name);
}
	

void on_response(int status, const char response[]) {
	if (status < 0) {
		printf("error: %s\n", response);
		state = Error;
		return;
	}
	switch(state) {
		case Start:
			if (status != STATUS_OK) {
				printf("error: %s\n", response);
				state = Done;
			}
			else {
				printf("response: %s\n", response);
				state = CreateGame;
				create_game(GAME_NAME, 2);
			}
			break;
		case CreateGame:
			if (status != STATUS_OK) {
				printf("error: %s\n", response);
				if (status == ERR_TOPIC_DUPLICATE) {
					get_opponent_from_response(response, opponent_name);
					turn = OPPON_TURN;
					state = JoinGame;
					
					join_game(GAME_NAME);
				}
				else {
					state = Error;
				}
			}
				
			else {
				printf("I start game!\n");
				turn = MY_TURN;
				show_curr_player();
				
				state = WaitPartner;
			
			}
			break;
		case JoinGame:
			if (status != STATUS_OK) {
				state = Error;
			}
			else {
				printf("Opponent start game!\n");
				printf("game is active!\n");
				state = InGame;
				turn = OPPON_TURN;
				show_curr_player();
			}	
			break;
		default:
			break;
	}
						
}


void timer_handler() {
	if (turn == MY_TURN && state == InGame)
		if (demo_mode) play_auto();
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
	 
	game_session = gs_connect(server_ip, username, on_response, on_msg);
	printf("\nStart!\n\n");
	graph_start();
	return 0;
}
