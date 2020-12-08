
#pragma once 


// error 

#define STATUS_ERROR	-1


// limits
#define MAX_NAME_SIZE	32
 
// Command types

#define CREATE_GAME "CREATE_TOPIC"
#define JOIN_GAME   "JOIN_TOPIC"
#define LEAVE_GAME   "LEAVE_TOPIC"
#define PLAY		"BROADCAST"
#define REMOVE_GAME "REMOVE_TOPIC"
#define DESTROY_GAME "DESTROY_TOPIC"

// Notification messages

#define ENTER_PARTNER 		"ENTER_PARTNER"
#define LEAVE_PARTNER 		"LEAVE_PARTNER"
#define BROADCAST	  		"BROADCAST"
#define MESSAGE		  		"MESSAGE"
#define TOPIC_DESTROYED	  	"TOPIC_DESTROYED"
#define OPPONENT_LEAVE 		"OPPONENT_LEAVE"

typedef  void (*ResponseEventHandler)(int status, const char response[]);
typedef  void (*MsgEventHandler)(const char sender[], const char msg[]);


/**
 * retrieve the command arguments
 */
int get_args(char user[], char srv_addr[], char game[]); 

/**
 * game server connection   
 */
session_t bs_connect(const char gs_ip_addr[], 
					 const char user[],
					 ResponseEventHandler on_resp,
					 MsgEventHandler on_msg);
					 
/**
 * do our play
 */
void bs_play(session_t session, const char args[]);


/**
 * send the result for our opponent play
 */
void bs_send_result(session_t game_session, const char args[]);


/**
 * game creation operation exported by comm layer
 */
void bs_new_game(session_t session, const char *game);
	



/**
 * terminate game
 */
void bs_end_game(session_t session);

				 
/**
 * terminate session
 */
void bs_close_session(session_t session);
