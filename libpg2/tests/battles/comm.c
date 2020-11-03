
#include "pg/graphics.h"
#include "battleship.h"

enum new_game_state { GameCreation, WaitingPartner, JoiningGame, Done, Error };

typedef struct context {
	enum new_game_state state;
	comm_cb_t old_comm_cb;
	session_t session;
	char *game_name;
	char opponent_name[64];
	int rank;
} context_t;

static void on_msg(const char sender[], const char msg[], void*context);
static void on_response(int status, const char response[], void *context);

context_t *create_context(session_t session, const char *game_name) {
	context_t *ctx = malloc(sizeof(context_t));
	
	ctx->old_comm_cb = gs_force_callbacks(on_response, on_msg);
	ctx->state = GameCreation;
	ctx->session = session;
	ctx->game_name = strdup(game_name);
	ctx->rank = 0;
	return ctx;
}

void create_game(session_t session, const char *game, int nplayers) {
	char args[256];
	int port = session_get_msg_port(session);
	sprintf(args, "battleship %s %d\n%d\n\n",  game, nplayers, port);
	gs_request(session, CREATE_GAME, args);
}

void join_game(session_t session, char *game) {
	char args[256];
	int port = session_get_msg_port(session);
	sprintf(args, "battleship %s\n%d\n\n",  game, port);
	gs_request(session, JOIN_GAME, args);
}

static void connection_abort(int status, const char *error_msg, context_t *ctx) {
	gs_force_callbacks(ctx->old_comm_cb.on_response, ctx->old_comm_cb.on_msg);
	ctx->session->on_response(status, error_msg);
	free(ctx->game_name);
	free(ctx);
}	

static void done(context_t *ctx) { 
	char resp[256];
	
	gs_force_callbacks(ctx->old_comm_cb.on_response, ctx->old_comm_cb.on_msg);
	// Response message
	// 201 Status Ok
	// <opponent_name> <game_name> <rank>
	sprintf(resp, "%s %d\n\n", ctx->opponent_name,   ctx->rank);
	ctx->session->on_response(STATUS_OK, resp);
	free(ctx->game_name);
	free(ctx);
}

static void on_msg(const char sender[], const char msg[], void * _ctx) {
	context_t *ctx = (context_t *) _ctx;
	 printf("msg send from group joiner %s: %s\n", sender, msg);
	 
	 if (ctx->state == WaitingPartner) {
		 printf("game is active!\n");
		 strcpy(ctx->opponent_name, sender);
		 ctx->rank = 1;
		 done(ctx);
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



static void on_response(int status, const char response[], void *_ctx) {
	context_t *ctx = (context_t *) _ctx;
	if (status < 0) {
		connection_abort(status, response, ctx);
		return;
	}
	switch(ctx->state) {
		case GameCreation:
			if (status != STATUS_OK) {
				if (status == ERR_TOPIC_DUPLICATE) {
					get_opponent_from_response(response, ctx->opponent_name);
					ctx->state = JoiningGame;	
					join_game(ctx->session, ctx->game_name);
				}
				else {
					connection_abort(status, response, ctx);
				}
			}
				
			else {
				printf("I start game!\n");	
				ctx->state = WaitingPartner;
			}
			break;
		 
		case JoiningGame:
			if (status != STATUS_OK) {
				connection_abort(status, response, ctx);
			}
			else {
				printf("Opponent start game!\n");
				printf("game is active!\n");
				ctx->rank = 2;
				done(ctx);
			}	
			break;
			
		default:
			break;
	}
						
}


void gs_new_game(session_t session, const char *game) {
	session->context = create_context(session, game);
	create_game(session, game, 2);
}
	


