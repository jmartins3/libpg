
/**
 * This module solves the battleship games creation and destruction protocols
 * in order to simplify the student process and help to keep the server in a 
 * sane state
 * 
 * It's an adapter between the pglib communication services and the program model exposed to students
 */
  
#include "pg/pglib.h"
#include <stdbool.h>
#include <unistd.h>
#include "battleship2.h"
#include "strutils.h"
#include "comm.h"
#include <assert.h>

//#define DEBUGR

// Warning Messages

#define BAD_NOTIFICATION_MSG 			"bad opponent message"
#define BAD_SHOT_MESSAGE				"bad opponent play"
#define BAD_NOTIFICATION_CMD_LINE 		"bad notification command line"
#define BAD_ENTER_PARTNER_PARMS 		"bad enter partner parameters"
#define BAD_LEAVE_PARTNER_PARMS 		"bad leave partner parameters"
#define BAD_GAME 						"wrong game name"
#define BAD_STATE 						"msg send/received in the wrong state"
#define BAD_NOTIFICATION_MSG_TYPE		"unknown notification type"
#define BAD_OPPONENT_NAME				"wrong opponent name"
#define BUSY_MSG 						"A command is being processed"



enum new_game_state { 
	
	WaitingResult	=	1,
	Connecting 		= 	2, 
	Connection		= 	4,
	GameCreation 	= 	8, 
	WaitingPartner 	= 	16, 
	JoiningGame		=	32,
	InGame			= 	64, 
	GameOver		=	128,
	EndingSession	= 	256,
	SessionDestroyed=	512,
	SessionClosed	= 	1024, 
	Error 			=	2048
};

#define ALL_STATES ( \
	Connecting | Connection | GameCreation | WaitingPartner | JoiningGame | InGame | GameOver | \
	EndingSession | SessionClosed | Error )

static session_t session;

typedef struct game_context {
	enum new_game_state state;			// the state of the game for comm layer
	session_t session;					// server session descriptor
	char *game_name;					// the battleship  game name
	ResponseEventHandler on_resp;		// response app callback
	char username[MAX_NAME_SIZE];		// the current player
	MsgEventHandler on_msg;				// message app callback;
	char opponent_name[MAX_NAME_SIZE];	// the opponent name
	int npartners;						// number of partners
	int rank;							// the player order (1,2)
	bool closing_window;				// thw game windows was closed
	bool pending_close;					// close session pending
	int pending_results;
} game_context_t;

// globals

// the active contexts list
// for no just one context (game) active
//

// the adapter layer callbacks

static void on_msg(const char msg[], void  *context);
static void on_response(int status, const char response[], void *context);


#define MAX_SIZE_IP_ADDR	15

int get_args(char user[], char srv_addr[], char game[]) {
	FILE *fargs;
	 
	if ((fargs = fopen("/proc/self/cmdline", "r")) == NULL) {	 
		return 0;
	}
	 
	char line[512];
	
	if (fgets(line, 512, fargs) == NULL)
		return -1;
	
	line[511] = 0;
	line[510] = 0;
	
	char *argv = line;
	int argc = 0;
	int len, actual_len;
	
	while ((len = strlen(argv)) > 0) {
		actual_len = len;
		switch(argc) {
			case 1: 
				if (len >= MAX_NAME_SIZE) len = MAX_NAME_SIZE -1;
				strcpy(user, argv);
				break;
			case 2:
				if (len >= MAX_SIZE_IP_ADDR) len = MAX_SIZE_IP_ADDR -1;
				strcpy(srv_addr, argv);
				break;
			case 3:
				if (len >= MAX_NAME_SIZE) len = MAX_NAME_SIZE -1;
				strcpy(game, argv);
				break;
			default:
				break;
		}
		argc++;
		argv += actual_len + 1;
	}
		
	fclose(fargs);
	return argc-1;

}


// the context acessors and constructors

void game_context_set_name(game_context_t *ctx , const char *game_name) {
 	ctx->game_name = strdup(game_name);	 
}

void game_context_set_opponent(game_context_t *ctx , const char *opponent_name) {
 	 strcpy(ctx->opponent_name, opponent_name);	 
 	 ctx->npartners = 1;
}


void game_context_set_rank(game_context_t *ctx, int rank) {
	ctx->rank = rank;
}

int game_get_state(game_context_t *ctx) {
	return ctx->state & ~WaitingResult;
}

bool game_wait_result(game_context_t *ctx) {
	return ctx->state & WaitingResult;
}

void game_reset_wait_result(game_context_t *ctx) {
	ctx->state &= ~WaitingResult;
}

game_context_t *game_context_create(const char *user, ResponseEventHandler on_resp, MsgEventHandler on_msg) {
	game_context_t *ctx = calloc(1, sizeof(game_context_t));
	
	ctx->on_resp = on_resp;
	ctx->on_msg = on_msg;
	ctx->state = Connecting;
	strcpy(ctx->username, user);
	ctx->closing_window = false;
	ctx->pending_close = false;
	ctx->pending_results = 0;
	return ctx;
}


// errors and warnings
static void connection_abort(int status, const char *error_msg, game_context_t *ctx) {
	if (ctx->on_resp != NULL) ctx->on_resp(status, error_msg);
	ctx->state = Error;
}	

static void warning(const char* warning_msg) {
	printf("warning: %s!\n", warning_msg);
}

bool check_valid(game_context_t *ctx, 
				int valid_states) {

	if ((ctx->state & valid_states) == 0) {
		connection_abort(STATUS_ERROR, BAD_STATE, ctx);
		return false;
	}
 
	return true;
}

void dispatch_resp(game_context_t *ctx, int status, const char resp[], int new_state) {
	char error_msg[100];
	const char *r = resp;
	
	if (status != STATUS_OK) {
		// remove the line terminators from response
		str_next_line(resp, 0, error_msg, 100);
		r = error_msg;
	
		ctx->state = Error; 
	}
	else ctx->state = new_state;
	
	if (ctx->on_resp != NULL) ctx->on_resp(status, r);
}

// internal requestors for group server

void create_game(session_t session, const char *game, int nplayers) {
	char args[256];
	int port = session_get_msg_port(session);
	sprintf(args, "battleship %s %d\n%d\n\n",  game, nplayers, port);
	game_context_t *ctx = (game_context_t *) session->context;
	ctx->pending_results++;
	gs_request(session, CREATE_GAME, args);
}

void list_themes(session_t session) {
 	 
	game_context_t *ctx = (game_context_t *) session->context;
	ctx->pending_results++;
	gs_request(session, LIST_THEMES, "\n\n");
}


void remove_game(session_t session, const char *game) {
	char args[256];
	 
	sprintf(args, "battleship %s\n\n",  game);
	game_context_t *ctx = (game_context_t *) session->context;
	ctx->pending_results++;
	gs_request(session, REMOVE_GAME, args);
}

void join_game(session_t session, char *game) {
	char args[256];
	game_context_t *ctx = (game_context_t *) session->context;
	int port = session_get_msg_port(session);
	sprintf(args, "battleship %s\n%d\n\n",  game, port);
	ctx->pending_results++;
	gs_request(session, JOIN_GAME, args);
}

void leave_game(session_t session, char *game) {
	char args[256];
	 
	sprintf(args, "battleship %s\n\n",  game);
	game_context_t *ctx = (game_context_t *) session->context;
	ctx->pending_results++;
	gs_request(session, LEAVE_GAME, args);
}


void destroy_game(session_t session, char *game) {
	char args[256];
	game_context_t *ctx = (game_context_t *) session->context;
	sprintf(args, "battleship %s\n\n",  game);
	ctx->pending_results++;
	gs_request(session, DESTROY_GAME, args);
}


// auxiliary protocol methods and callbacks

/**
 * start game app callback dispatcher
 */
static void game_started(game_context_t *ctx) { 
	char resp[256];
	
	// Virtual response message to application
	// 201 Status Ok
	// <opponent_name> <game_name> <rank>
	sprintf(resp, "%s %d\n\n", ctx->opponent_name,   ctx->rank);
	
	dispatch_resp(ctx, STATUS_OK, resp, InGame);
}

/**
 * Auxiliary method to obtain the notification message context:
 * sender, theme, topic, number of partners
 */
static int get_msg_parms(const char msg[], int start, char sender[], 
				                 char theme[], char topic[], int *partners,
				                 game_context_t *ctx) {
	char line[2];
	start = str_next_line(msg, start, line, 2);
	if (start == -1) { warning(BAD_NOTIFICATION_CMD_LINE); return -1; }
	start = str_next(msg, start, sender, MAX_NAME_SIZE, All);
	if (start != -1) start = str_next(msg, start, theme, MAX_NAME_SIZE, All);
	if (start != -1) start = str_next(msg, start, topic, MAX_NAME_SIZE, All);
	if (start != -1) start = str_next_int(msg, start, partners);
	if (start != -1) start = str_next_line(msg, start, line, 2);
	if (start == -1) { warning(BAD_ENTER_PARTNER_PARMS);  return -1; }
	if (strcmp(ctx->game_name, topic) != 0) { warning(BAD_GAME); return -1; }
	return start;	
}

/* ENTER_PARTNER <LF>
 * <username>  <theme>  <topic> <total_joiners><LF>
 * <LF>
 */ 
static void  process_enter_partner(const char *msg, int start, game_context_t *ctx) {
	char sender[MAX_NAME_SIZE], theme[MAX_NAME_SIZE], topic[MAX_NAME_SIZE];
	int partners;
	
	if ((start = get_msg_parms(msg, start, sender, theme, topic, &partners, ctx)) == -1)
		return;
		
	switch (game_get_state(ctx)) {
	  case  WaitingPartner:
			
			printf("game is active!\n");
			game_context_set_opponent(ctx, sender);
			ctx->npartners = 1;
			game_started(ctx);
			break;
	  default:
			warning(BAD_STATE);
			break;
	}
}

/* LEAVE_PARTNER <LF>
 * <username>  <theme>  <topic> <total_joiners><LF>
 * <LF>
 */ 
static void process_leave_partner(const char *msg, int start, game_context_t *ctx) {
	char sender[MAX_NAME_SIZE], theme[MAX_NAME_SIZE], topic[MAX_NAME_SIZE];
	int partners;
	char notific[256];
	
	if ((start = get_msg_parms(msg, start, sender, theme, topic, &partners, ctx)) == -1)
		return;
#ifdef DEBUG
	printf("in leave partner, state = %x\n", game_get_state(ctx));
#endif
	switch (game_get_state(ctx)) {
	  case  InGame:
			ctx->npartners = 0;
			warning("opponent leave game\n");
			if (ctx->state < GameOver) {
				ctx->state = GameOver;
				remove_game(ctx->session, ctx->game_name);
			}
			break;
	  
	  default:
			 
			break;
	
	  if (ctx->on_msg != NULL) {
		  sprintf(notific, "%s\n%s %s\n\n", OPPONENT_LEAVE, sender, topic);
		  ctx->on_msg(sender, notific);
	  }
	}
}

/* BROADCAST <LF>
 * <username>  <theme>  <topic> <total_joiners><LF>
 * <LF>
 */
static void process_broadcast(const char *msg, int start, game_context_t *ctx) {
	char sender[MAX_NAME_SIZE], theme[MAX_NAME_SIZE], topic[MAX_NAME_SIZE];
	int partners;
	
	if ((start = get_msg_parms(msg, start, sender, theme, topic, &partners, ctx)) == -1)
		return;
	
	if (game_get_state(ctx) < InGame) {warning(BAD_STATE); return; }
 
	if (strcmp(ctx->opponent_name, sender) != 0) warning(BAD_OPPONENT_NAME);
	else ctx->on_msg(sender, msg+start);
	
	game_reset_wait_result(ctx);
	 
}

/* MESSAGE <LF>
 * <username>  <theme>  <topic> <total_joiners><LF>
 * <LF>
 */
static void process_message(const char *msg, int start, game_context_t *ctx) {
	char sender[MAX_NAME_SIZE], theme[MAX_NAME_SIZE], topic[MAX_NAME_SIZE];
	int partners;
	
	if ((start = get_msg_parms(msg, start, sender, theme, topic, &partners, ctx)) == -1)
		return;
	 
	switch (game_get_state(ctx)) {
	  case  InGame:
			if (strcmp(ctx->opponent_name, sender) != 0) warning(BAD_OPPONENT_NAME);
			else ctx->on_msg(sender, msg+start);
			break;
	  default:
			warning(BAD_STATE);
			break;
	}
}

/* TOPIC_DESTROYED <LF>
 * <username>  <theme>  <topic> <total_joiners><LF>
 * <LF>
 */
static void process_destroyed_topic(const char *msg, int start, game_context_t *ctx) {
	char sender[MAX_NAME_SIZE], theme[MAX_NAME_SIZE], topic[MAX_NAME_SIZE];
	int partners;
	
	if ((start = get_msg_parms(msg, start, sender, theme, topic, &partners, ctx)) == -1)
		return;
	 
	 
	ctx->npartners = 0;
	if (ctx->state < GameOver) 
				ctx->state = GameOver;
	warning("owner destroyed game");
}
			
/**
 * general notification message processor
 */
static void prepare_dispatch_message(const char *msg, game_context_t *ctx) {
	// retrieve notification type
	char msg_type[32];
	
	int start = str_next(msg, 0, msg_type, 32, All);
	if (start == -1) { warning(BAD_NOTIFICATION_MSG); return; }
	
	if (strcmp(ENTER_PARTNER, msg_type) == 0)
		process_enter_partner(msg, start, ctx);
	else if (strcmp(LEAVE_PARTNER, msg_type) == 0)
		process_leave_partner(msg, start, ctx);
	else if (strcmp(BROADCAST, msg_type) == 0)
		process_broadcast(msg, start, ctx);	
	else if (strcmp(MESSAGE, msg_type) == 0)
		process_message(msg, start, ctx);
	else if (strcmp(TOPIC_DESTROYED, msg_type) == 0)
		process_destroyed_topic(msg, start, ctx);
	else
		warning(BAD_NOTIFICATION_MSG_TYPE);	
}

/**
 * The notification messages callback
 */
static void on_msg(const char msg[], void * _ctx) {
	game_context_t *ctx = (game_context_t *) _ctx;
#ifdef DEBUGN
	printf("msg received from group joiner: %s\n",  msg);
#endif
	prepare_dispatch_message(msg, ctx);
}


/**
 * Auxiliary method used on we are the second player
 * by the protocol to obtain the game creator and our opponent
 * from the create game response
 */
static void get_opponent_from_response(const char *resp, char *opponent_name) {
	char line[2];
	int start = str_next_line(resp, 0, line, 2);
	start = str_next_word(resp, start, opponent_name, MAX_NAME_SIZE);
	if (start == -1) warning(BAD_OPPONENT_NAME);
	printf("opponent_name=%s\n", opponent_name);
}


/**
 * Callback for the adaptation layer
 * This callback  contains the state machine to process the 
 * game creation and termination protocol and forwards the messages to the 
 * application callback for the session construction response and 
 * when the game is active
 */ 
static void on_response(int status, const char response[], void *_ctx) {
	game_context_t *ctx = (game_context_t *) _ctx;
#ifdef DEBUGR
	printf("response callback in state %d: '%s' for %s\n", ctx->state, response, ctx->username);
	printf("%s, pending_results =%d, pending_close=%d\n", ctx->username, ctx->pending_results, ctx->pending_close);
#endif

		
	ctx->pending_results--;
	 	


	assert((!ctx->pending_close && ctx->pending_results  <=1)
	       || (ctx->pending_close && ctx->pending_results <= 2));
	       
	 

	switch(game_get_state(ctx)) {
		case Connecting:
			if (status != STATUS_OK) 
				connection_abort(status, response, ctx);
			else {
				ctx->state = Connection;
				list_themes(session);
			}
				
			break;
		case Connection:
		 	
			// forward response to application callback and change state to GameCreation
			
			dispatch_resp(ctx, status, response, GameCreation);
			break;
		case InGame:
			// forward response
			dispatch_resp(ctx, status, response, InGame);
			 
			break;
		case GameCreation:
			
			if (status != STATUS_OK) {
				printf("error %d: '%s' creating game for %s\n", status, response, ctx->username);
				if (status == ERR_TOPIC_DUPLICATE) {
					get_opponent_from_response(response, ctx->opponent_name);
					ctx->state = JoiningGame;	
					ctx->rank = 2;
					join_game(ctx->session, ctx->game_name);
				}
				else {
					connection_abort(status, response, ctx);
				}
			}	
			else {
				ctx->state = WaitingPartner;
				ctx->rank = 1;
			}
			break;
		 
		case JoiningGame:
			if (ctx->state == EndingSession) {
				return;
			}
			if (status != STATUS_OK) {
				connection_abort(status, response, ctx);
			}
			else {
				ctx->npartners = 1;
				game_started(ctx);
			}	
			break;
		case EndingSession:
#ifdef DEBUGC	
			printf("Ending session, response '%s' on game for %s\n", response, ctx->username);
			printf("on ending session, pending_results =%d, pending_close=%d\n", ctx->pending_results, ctx->pending_close);
#endif    
			if (ctx->pending_results == 0) {
				if (ctx->state >= SessionDestroyed) return;
				ctx->state = SessionDestroyed;	
				ctx->pending_results++;
				gs_session_destroy(ctx->session);
			}
				
			break;
	
		case SessionDestroyed:
			assert(session->chn == NULL);
		 
			if (ctx->state == SessionClosed) return;
			ctx->state = SessionClosed;
#ifdef DEBUGC
			printf("Session closed for %s!\n", ctx->username);
#endif
			if (ctx->closing_window)
				graph_exit();
			break;
		default:
		 
			break;
	}	
	

	// try to restart close session
	if (ctx->pending_results == 0 && ctx->pending_close) {
		ctx->pending_close=false;
		bs_close_session(ctx->session);
	}
		
}

/**
 * battleship play command exported by comm layer
 */
void bs_play(session_t session, const char args[]) {
	char cmd[256];
	game_context_t *ctx = (game_context_t *) session->context;
	int valid_states = (ctx->state & WaitingResult) ? 0 : InGame;
	if (check_valid(ctx, valid_states)) {
		sprintf(cmd, "battleship %s\n%s\n", ctx->game_name, args);
		ctx->pending_results++;
		gs_request(session, BROADCAST, cmd);
		ctx->state |= WaitingResult;
	}
}


void bs_send_result(session_t game_session, const char args[]) {
	char cmd[256];
	game_context_t *ctx = (game_context_t *) session->context;
#ifdef DEBUG
	printf("bs_send_result: state=%d\n", game_get_state(ctx));
#endif
	int valid_states= InGame;
	
	if (!check_valid(ctx, valid_states)) {
		return;
	}
#ifdef DEBUG
    printf("bs_send_result: exec\n");
#endif
    
	sprintf(cmd, "battleship %s\n%s\n", ctx->game_name, args);
	ctx->pending_results++;
	gs_request(session, BROADCAST, cmd);
}

/**
 * game creation operation exported by comm layer
 */
void bs_new_game(session_t session, const char *game) {
	game_context_t *ctx = (game_context_t *) session->context;
	
	int valid_states = GameCreation;
 
	if (check_valid(ctx, valid_states)) {
		game_context_set_name(ctx, game);
		create_game(session, game, 2);
	}
}

	
/**
 * game server connection operation exported by commm layer
 */
session_t bs_connect(const char ip_addr[], 
					 const char user[],
					 ResponseEventHandler on_resp,
					 MsgEventHandler on_message) {
	
	if (session != NULL)  
		 on_resp(-1, "Connection already Done");
	else {
		game_context_t *ctx = game_context_create( user, on_resp,on_message);
		ctx->pending_results++;
		session = gs_connect(ip_addr, user, on_response, on_msg, ctx);
		ctx->session = session;
	}
	 
	return session;
}
 

void bs_end_game(session_t session) {
	game_context_t *ctx = (game_context_t *) session->context;
 	
	
	if (ctx->rank == 1 || ctx->state  == WaitingPartner) { // I'm the owner of the game
		ctx->state = GameOver;
		if (ctx->npartners == 0)
			remove_game(session, ctx->game_name);
		
	}
	else if (ctx->rank ==2) {
		if (ctx->npartners == 1) {
			ctx->state = GameOver;
			ctx->npartners = 0;
			leave_game(session, ctx->game_name);
		}
	}			
}

void bs_close_session(session_t s) {
	if (session == NULL || session != s) return;
	game_context_t *ctx = (game_context_t *) session->context;
	if (ctx->pending_results > 0) {
		ctx->pending_close=true;
		return;
	}
	int state = game_get_state(ctx);
#ifdef DEBUGC
printf("ending session in state %d\n", state);
#endif
	if (state == Connecting || state == GameCreation) {
		ctx->state = EndingSession;
	}
	else if (state == Connection) {
		ctx->state = SessionDestroyed;	
		gs_session_destroy(session);
	}
	else if (state == JoiningGame) {
		leave_game(session, ctx->game_name);
		ctx->state = EndingSession;
	}
	else if (state == WaitingPartner) {
		destroy_game(session, ctx->game_name);
		ctx->state = EndingSession;
	}
	else if (state == InGame || state == GameOver) {
		ctx->state = EndingSession;
		if (ctx->rank == 1) {
			if (ctx->npartners == 1)
				destroy_game(session, ctx->game_name);
			else {
				ctx->state = SessionDestroyed;	
				ctx->pending_results++;
				gs_session_destroy(ctx->session);
			}
		}
		else {
#ifdef DEBUGC
			printf("leave game on end session, pending_results=%d!\n",
				ctx->pending_results);
#endif
			if (ctx->npartners == 1)
				leave_game(session, ctx->game_name);
			else {
				ctx->state = SessionDestroyed;	
				ctx->pending_results++;
				gs_session_destroy(ctx->session);
			}
		}
	
	}
	else if (state == Error) {
		ctx->state = SessionDestroyed;
		gs_session_destroy(session);
	}
	
}


bool onEnd() {
	game_context_t *ctx = (game_context_t *) session->context;
	if ( ctx->state >= EndingSession) {
#ifdef DEBUGC
		printf("battleship for %s terminated!\n", ctx->username);
#endif
		return true;
	}
	ctx->closing_window = true;
	bs_close_session(session);
	return false;
}
