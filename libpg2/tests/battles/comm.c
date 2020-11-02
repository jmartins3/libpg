
#include "pg/graphics.h"


enum new_game_state { Creation, WaitingPartner, Joining, Done, Error };

static enum new__game_state state;
static struct comm_cb  old_comm_cb;
static session_t session;
static const char *game_name;
static opponent_game[64];

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


static void abort(int status, const char *error_msg) {
	gs_force_callbacks(old_comm_cb.on_response, old_comm_cb.on_msg);
	printf("error: %s\n", response);
	session->on_response(status, error_msg);
	state = Error;
}	

static void done(

static void on_msg(const char sender[], const char msg[]) {
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



static void on_response(int status, const char response[]) {
	if (status < 0) {
		abort(status, response);
		return;
	}
	switch(state) {
		case Creation:
			if (status != STATUS_OK) {
				printf("error: %s\n", response);
				if (status == ERR_TOPIC_DUPLICATE) {
					get_opponent_from_response(response, opponent_name);
					turn = OPPON_TURN;
					state = JoinGame;
					
					join_game(GAME_NAME);
				}
				else {
					abort(status, "Unexpected error creating game");
				}
			}
				
			else {
				printf("I start game!\n");	
				state = WaitingPartner;
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


void gs_new_game(session_t _session, const char *_game) {
	
	old_comm_cb = gs_force_callbacks(on_response, on_msg);
	state = Creation;
	session = _session;
	game_name = strdup(_game);
	create_game(_game, 2);
	
}
	


