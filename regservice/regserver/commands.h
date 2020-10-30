#pragma once

#include "../include/reg_service.h"
#include <stdbool.h>
#include "../include/list.h"
#include "repository.h"
#include "limits.h"



enum command { Regist, Unregist, CreateTheme, CreateTopic, ListThemes, 
			ListTopics, RemoveTheme, RemoveTopic, JoinTopic, Unknown,
			LeaveTopic, Broadcast, Message, Stop};

typedef enum command  CmdType;
 


struct channel;

typedef struct cmd  {
	CmdType type; 						// for command introspection
	struct channel *chn;				// the command context
	user_t *user;	
	int argline;						// the arguments current line
} Cmd;

typedef struct cmd_regist {
	Cmd base; 							// for command introspection
	int number;							// the user(student) number
	char username[MAX_USER_NAME+1];		// the user name
	char passwd[MAX_PASSWORD_SIZE+1];	// the user password
} CmdRegist;

typedef struct cmd_unregist {
	Cmd base; 							// for command introspection
} CmdUnregist;

typedef struct cmd_create_theme {
	Cmd base; 							// for command introspection
	char theme[MAX_THEME_NAME];			// new theme name
} CmdCreateTheme;

typedef struct cmd_create_topic {
	Cmd base; 							// for command introspection
	char theme[MAX_THEME_NAME];			// the theme of the new topic
	char topic[MAX_TOPIC_NAME];			// new topic name
	int port;							// udp port for messaging
	int limit;
} CmdCreateTopic;

typedef struct cmd_list_themes {
	Cmd base; 							// for command introspection
} CmdListThemes;

typedef struct cmd_list_topics {
	Cmd base; 							// for command introspection
	char theme[MAX_THEME_NAME];			// the theme of topics list
} CmdListTopics;

typedef struct cmd_remove_theme {
	Cmd base; 							// for command introspection
	char theme[MAX_THEME_NAME];			// the theme of topics list
} CmdRemoveTheme;

typedef struct cmd_remove_topic {
	Cmd base; 							// for command introspection
	char theme[MAX_THEME_NAME];			// the theme of the topic
	char topic[MAX_TOPIC_NAME];			// topic name to remove
} CmdRemoveTopic;

typedef struct cmd_join_topic {
	Cmd base; 							// for command introspection
	char theme[MAX_THEME_NAME];			// the theme of the topic
	char topic[MAX_TOPIC_NAME];			// topic name to join
	int port;							// udp port for messaging
	int njoiners;						// total topic joiner after new join
} CmdJoinTopic;

typedef struct cmd_leave_topic {
	Cmd base; 							// for command introspection
	char theme[MAX_THEME_NAME];			// the theme of the topic
	char topic[MAX_TOPIC_NAME];			// topic name to leave
	int njoiners;						// total topic joiner after leave
} CmdLeaveTopic;


#define MAX_BCAST_MSG_LINES		32

#define MAX_SENDER_INFO (MAX_USER_NAME+ MAX_THEME_NAME + MAX_TOPIC_NAME + 4)

typedef struct req_array_entry {
	uv_udp_send_t req;
	int idx;
} req_array_entry_t;

typedef struct cmd_broadcast {
	Cmd base; 							// for command introspection
	char theme[MAX_THEME_NAME];			// the theme of the new topic
	char topic[MAX_TOPIC_NAME];			// new topic name 
	int nlines;
	char *lines[MAX_BCAST_MSG_LINES];	// array of msg lines pointers
	int sz_lines[MAX_BCAST_MSG_LINES];	// array of line sizes
	char send_info[MAX_SENDER_INFO];	// header info for udo msgs (sender theme topic total)
	req_array_entry_t *reqs;			// requests array
	int total_partners;					// total partners to send (topic joiners - 1)
	int send_partners;					// the msgs known as sended
} CmdBroadcast;	

typedef struct cmd_message {
	Cmd base; 							// for command introspection
	char theme[MAX_THEME_NAME];			// the theme of the new topic
	char topic[MAX_TOPIC_NAME];			// new topic name 
	char user_dest[MAX_USER_NAME];		// user destinatoion
	int nlines;
	char *lines[MAX_BCAST_MSG_LINES];	// array of msg lines pointers
	int sz_lines[MAX_BCAST_MSG_LINES];	// array of line sizes
	char send_info[MAX_SENDER_INFO];	// header info for udp msgs (sender theme topic total)
	uv_udp_send_t req;					// requests array
} CmdMsg;	

typedef int (*ArgsReader)(Cmd * _cmd, const char *line);


/// Answers


typedef struct  {
	int status;
	union {
		int njoiners;
		names_result_t names;
		char *username;
	};
} Answer;

/*
 * initialize commands stuff
 */
void cmds_init(uv_loop_t *loop);


ArgsReader cmd_args_reader(Cmd *cmd);


void inc_frees();
void inc_mallocs();
void show_allocations();


/*
 * cmommand creator
 */
Cmd *cmd_create(const char *cmd_name, struct channel *chn);

void cmd_destroy(Cmd *cmd);

/*
 * registration  command
 */
void cmd_reg(Cmd * _cmd);
	 

/*
 * unregistration  command
 */
void cmd_unreg(Cmd * _cmd);
	 
/* Create theme command
 */
void cmd_create_theme(Cmd *_cmd);
	 

/*
 * Create topic command
 */
void cmd_create_topic(Cmd *_cmd); 

/*
 * list themes command
 */
void cmd_list_themes(Cmd *_cmd);
	 


/*
 * lLst topics command
 */
void cmd_list_topics(Cmd *_cmd);
 


/*
 * remove theme command
 */
void cmd_remove_theme(Cmd *_cmd);
	 

/*
 * remove topic command
 */
void cmd_remove_topic(Cmd *_cmd);
 


/*
 * join topic command
 * int uv_tcp_getpeername(const uv_tcp_t* handle, struct sockaddr* name, int* namelen)
 */
void cmd_join_topic(Cmd *_cmd);
 


/*
 * leave topic command
 */
void cmd_leave_topic(Cmd *_cmd);
	 
 
/*
 * broadcast command
 */
void cmd_broadcast(Cmd *_cmd);
	 
/*
 * send a unicast message
 */
void cmd_message(Cmd *_cmd);

void cmd_exec(Cmd *cmd);
