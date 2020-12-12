
#pragma once 




// Command types

#define CREATE_GAME 		"CREATE_TOPIC"
#define JOIN_GAME   		"JOIN_TOPIC"
#define LEAVE_GAME   		"LEAVE_TOPIC"
#define PLAY				"BROADCAST"
#define REMOVE_GAME 		"REMOVE_TOPIC"
#define DESTROY_GAME 		"DESTROY_TOPIC"
#define LIST_THEMES			"LIST_THEMES"

// Notification messages

#define ENTER_PARTNER 		"ENTER_PARTNER"
#define LEAVE_PARTNER 		"LEAVE_PARTNER"
#define BROADCAST	  		"BROADCAST"
#define MESSAGE		  		"MESSAGE"
#define TOPIC_DESTROYED	  	"TOPIC_DESTROYED"
#define OPPONENT_LEAVE 		"OPPONENT_LEAVE"
#define OPPONENT_ENTER 		"OPPONENT_ENTER"
#define GAME_DESTROYED		"GAME_DESTROYED"

typedef  void (*ResponseEventHandler)(int status, const char response[]);
typedef  void (*MsgEventHandler)(const char sender[], const char msg[]);




#ifdef __cplusplus 
extern "C" {
#endif


/**
 * retrieve the command arguments
 */
int get_args(char user[], char srv_addr[], char game[]); 

/**
 * game server connection   
 */
session_t srv_connect(const char gs_ip_addr[], 
					 const char user[],
					 ResponseEventHandler on_resp,
					 MsgEventHandler on_msg);
					 
/**
 * do our play
 */
void srv_play(session_t session, const char args[]);


/**
 * send the result for our opponent play
 */
void srv_send_result(session_t game_session, const char args[]);


/**
 * game creation operation exported by comm layer
 */
void srv_new_game(session_t session, const char game_type[], const char game_name[]);
	



/**
 * terminate game
 */
void srv_end_game(session_t session);

				 
/**
 * terminate session
 */
void srv_close_session(session_t session);



#ifdef __cplusplus 
}	
#endif
