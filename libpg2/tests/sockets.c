#include <stdio.h>
#include "pg/graphics.h"

typedef enum state { Start, CreateGame, Done } state_t;

session_t game_session;
state_t state;


void create_game(char *game, int nplayers) {
	char args[256];
	int port = session_get_msg_port(game_session);
	sprintf(args, "battleship %s %d\n%d\n\n",  game, nplayers, port);
	gs_request(game_session, CREATE_GAME, args);
}



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

									
int main() {
	graph_init();
	
	state = Start;
	
	game_session = gs_connect("127.0.0.1", "mary", "mary_pass", on_response, on_msg);
	graph_start();
	return 0;
}
