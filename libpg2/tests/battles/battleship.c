#include <stdio.h>

#include <stdlib.h>
#include <stdbool.h>

#include "pg/graphics.h"
#include "board.h"

#define MY_BOARD_X 50
#define MY_BOARD_Y 90

#define OPPON_BOARD_X 50
#define OPPON_BOARD_Y 430

#define MY_TURN		0
#define OPPON_TURN	1

#define MSG_PLAYER_X	400
#define MSG_PLAYER_Y	50
 
battleship_t battle, oppon_battle;

typedef enum state { Start, CreateGame, Done } state_t;

session_t game_session;
state_t state;

int turn; // MY_TURN or OPPON_TURN

typedef enum result { WATER, HIT, BAD } result_t;

void show_curr_player() {
	//Size text_size = graph_text_size("PLAYER ONE", LARGE_FONT);
	// erase
	//graph_rect(MSG_PLAYER_X, MSG_PLAYER_Y-text_size.height,text_size.width, text_size.height, c_white, true);
	 
	if (turn == MY_TURN) {
		graph_text(MSG_PLAYER_X, MSG_PLAYER_Y, c_gray,  "PLAYER ONE", LARGE_FONT);
	}
	else {
		graph_text(MSG_PLAYER_X, MSG_PLAYER_Y, c_gray,  "PLAYER TWO", LARGE_FONT);
	}
}


result_t play(int x, int y) {
	battleship_t *curr_battle, *other_battle;
	
	curr_battle = turn == MY_TURN ? &battle : & oppon_battle;
	other_battle = turn == MY_TURN ? &oppon_battle : &battle;
	 	
	Point p = place_selected(&curr_battle->oppon_board, x, y);
	if (p.x == -1) return BAD;
	
	int val = val_place(&other_battle->my_board, p.x, p.y);
	fill_place(&curr_battle->oppon_board, p.x, p.y, val);
	draw_place(&curr_battle->oppon_board, p.x, p.y);
	
	turn = (turn+1) % 2;
	show_curr_player();
	return val == EMPTY ? WATER : HIT;
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
	else {
		Point p = place_selected(&oppon_battle.my_board, x, y);
		if (p.x != -1) {
			if (oppon_board_visible) {
				hide_board(&oppon_battle.my_board);
				oppon_board_visible = false;
			}
			else {
				draw_board(&oppon_battle.my_board, true);
				oppon_board_visible = true;
			}
		}
	}
}
	
void mouse_handler(MouseEvent me) {
	if (me.type == MOUSE_BUTTON_EVENT && 
		me.state == BUTTON_PRESSED) {
		
		/*
		if (me.button == BUTTON_LEFT) {
			populate_board(&battle.my_board);
			draw_board(&battle.my_board);	
		}
		else {
			populate_board(&battle.oppon_board);
			draw_board(&battle.oppon_board);
		}
		*/
		if (me.button == BUTTON_LEFT) {
			play(me.x, me.y);
		}
		else { 
			toggle_board(me.x, me.y);
		}
			
	}
}

/*
void on_msg(const char sender[], const char msg[]) {
	 printf("msg send from group joiner: %s\n", msg);
}

void on_response(int status, const char response[]) {
	switch(state) {
		case Start:
			if (status != 0) {
				printf("error: %s\n", response);
				state = Done;
			}
			else {
				printf("response: %s\n", response);
				state = CreateGame;
				create_game("mary_battle_2", 2);
			}
			break;
		case CreateGame:
			if (status != 0) printf("error: %s\n", response);
			else {
				printf(" game creation response: %s\n", response);
			}
			state = Done;
			break;
		default:
			break;
	}
						
}

*/

	
int main() {
	srand(time(NULL));
	graph_init();
	
	create_battleship(&battle, MY_BOARD_X, MY_BOARD_Y, MY_BOARD_X+500, MY_BOARD_Y);
	create_battleship(&oppon_battle, OPPON_BOARD_X, OPPON_BOARD_Y, OPPON_BOARD_X+500, OPPON_BOARD_Y);
 
	populate_board(&battle.my_board);
	draw_board(&battle.my_board, false);
	draw_board(&battle.oppon_board, false);
	
	
	
	populate_board(&oppon_battle.my_board);
	draw_board(&oppon_battle.my_board, false);
	draw_board(&oppon_battle.oppon_board, false);
	
	//graph_refresh();
 	show_curr_player();
	graph_regist_mouse_handler(mouse_handler);
	//game_session = gs_connect("127.0.0.1", "mary", "mary_pass", on_response, on_msg);
	printf("\nStart!\n\n");
	graph_start();
	return 0;
}
