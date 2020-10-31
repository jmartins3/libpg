
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "strutils.h"
#include "repository.h"
#include "commands.h"
#include "responses.h"
#include "channel.h"

typedef Answer (*CmdHandler)(void *cmd);

#define UDP_SENDER_PORT 30001

// the udp socket used to send "group cast " messages
static uv_udp_t broadcaster; 

static int nmallocs, nfrees;

void inc_mallocs() {
	nmallocs++;
}

void inc_frees() {
	nfrees++;
}

void show_allocations() {
	printf("mallocs: %d, frees: %d\n", nmallocs, nfrees);
}


/*
 * Initialize commands stuff
 */
void cmds_init(uv_loop_t *loop) {
	struct sockaddr_in addr;
	uv_udp_init(loop, &broadcaster);
	uv_ip4_addr("0.0.0.0", UDP_SENDER_PORT, &addr);

    uv_udp_bind(&broadcaster, (const struct sockaddr*)&addr,0);
	db_init();
}


//
//
// Parameters
//

/*
 * represents an argument processor of a command
 * return possibilities:
 * 	< 0 for the symetric of a response status error
 *  = 0 if tehere are no more parameters for the associated command
 *  > 0 a subset of arguments have been correctly processed
 */
typedef int (*ArgsReader)(Cmd * _cmd, const char *line);

static user_t *getuser(const char *line) {
	int istart=0;
	char username[MAX_USER_NAME];
	char passwd[MAX_PASSWORD_SIZE];
	
	if ((istart = str_next(line, istart, username, MAX_USER_NAME)) < 0)
		return NULL;
	if ((istart = str_next(line, istart, passwd, MAX_PASSWORD_SIZE)) < 0)
		return NULL;	
	// check line termination
	if (!check_line_termination(line, istart)) return NULL;
	return user_get(username, passwd);
}


static int args_bad(Cmd * _cmd, const char *line) {
	return -BAD_COMMAND;
}

static int args_empty(Cmd*_cmd, const char *line) {
	if (_cmd->argline == 0) { // first line, authentication args
		if ((_cmd->user=getuser(line)) == NULL) return -BAD_USER_AUTH;
		_cmd->argline++;
		return 1;
	}
	else if (check_empty_line(line)) return 0;
	return -BAD_COMMAND;
}

static int  args_regist(Cmd * _cmd, const char *line) {
	
	CmdRegist *cmd = (CmdRegist *) _cmd;
	
	if (_cmd->argline==0) {
		int istart=0;
		char word[10];
		if ((istart = str_next(line, istart, word, 10)) < 0 ||
			(cmd->number = atoi(word)) <= 0) 
			return -BAD_COMMAND_ARGS;
		
		if ((istart = str_next(line, istart, cmd->username, MAX_USER_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		if ((istart = str_next(line, istart, cmd->passwd, MAX_PASSWORD_SIZE)) < 0)
			return -BAD_COMMAND_ARGS;	
		// check line termination
		if (!check_line_termination(line, istart))
			return -BAD_COMMAND_ARGS; 
		_cmd->argline++;
		return 1;
	}
	if (check_line_termination(line, 0)) return 0;
	return -BAD_COMMAND;
}



static int args_unregist(Cmd * _cmd, const char *line) {
 
	if (_cmd->argline == 0) { // first line, authentication args
		if ((_cmd->user=getuser(line)) == NULL) return -BAD_USER_AUTH;
		_cmd->argline++;
		return 1;
	}
	if (check_line_termination(line, 0)) return 0;
	return -BAD_COMMAND;
}

static int args_create_theme(Cmd * _cmd, const char *line) {
 	CmdCreateTheme *cmd = (CmdCreateTheme *) _cmd;
	if (_cmd->argline == 0) { // first line, authentication args
		if ((_cmd->user=getuser(line)) == NULL) return -BAD_USER_AUTH;
		_cmd->argline++;
		return 1;
	}
	else if (_cmd->argline == 1) { // second line, new theme
		int istart = 0;
		if ((istart = str_next(line, istart, cmd->theme, MAX_THEME_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		// check line termination
		if (!check_line_termination(line, istart)) 
			return -BAD_COMMAND_ARGS; 
		_cmd->argline++;
		return 1;
	}
	if (check_line_termination(line, 0)) return 0;
	return -BAD_COMMAND;
}

static int args_list_topics(Cmd * _cmd, const char *line) {
 	CmdListTopics *cmd = (CmdListTopics *) _cmd;
	
	if (_cmd->argline == 0) { // first line, authentication args
		if ((_cmd->user=getuser(line)) == NULL) return -BAD_USER_AUTH;
		_cmd->argline++;
		return 1;
	}
	else if (_cmd->argline == 1) { // second line,  theme to list
		int istart = 0;
		if ((istart = str_next(line, istart, cmd->theme, MAX_THEME_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		 
		// check line termination
		if (!check_line_termination(line, istart)) 
			return -BAD_COMMAND_ARGS; 
		_cmd->argline++;
		return 1;
	}
	if (check_line_termination(line, 0)) return 0;
	return -BAD_COMMAND;
}

static int args_list_users(Cmd * _cmd, const char *line) {
	if (check_line_termination(line, 0)) return 0;
	return -BAD_COMMAND;
}


static int args_create_topic(Cmd * _cmd, const char *line) {
 	CmdCreateTopic *cmd = (CmdCreateTopic *) _cmd;
	if (_cmd->argline == 0) { // first line, authentication args
		if ((_cmd->user=getuser(line)) == NULL) return -BAD_USER_AUTH;
		_cmd->argline++;
		return 1;
	}
	else if (_cmd->argline == 1) { // second line, new topic from theme
		char word[10];
		
		
		int istart = 0;
		if ((istart = str_next(line, istart, cmd->theme, MAX_THEME_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		if ((istart = str_next(line, istart, cmd->topic, MAX_TOPIC_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		// check line termination
		if (!check_line_termination(line, istart))  {
		 
			if ((istart = str_next(line, istart, word, 10)) < 0 ||
				(cmd->limit = atoi(word)) <= 0) 
				return -BAD_COMMAND_ARGS;
			if (!check_line_termination(line, istart))
				return -BAD_COMMAND_ARGS;
		}
		_cmd->argline++;
		return 1;
	}
	else if(_cmd->argline == 2)  { // third line, msg port
		int istart=0;
		char word[7];
		if ((istart = str_next(line, istart, word, 7)) < 0 ||
			(cmd->port = atoi(word)) <= 0) 
			return -BAD_COMMAND_ARGS;
		// check line termination
		if (!check_line_termination(line, istart)) 
			return -BAD_COMMAND_ARGS; 
		_cmd->argline++;
		return 1;
	}
	if (check_line_termination(line, 0)) return 0;
	return -BAD_COMMAND;
}


static int args_remove_theme(Cmd * _cmd, const char *line) {
 	CmdRemoveTheme *cmd = (CmdRemoveTheme *) _cmd;
	
	if (_cmd->argline == 0) { // first line, authentication args
		if ((_cmd->user=getuser(line)) == NULL) return -BAD_USER_AUTH;
		_cmd->argline++;
		return 1;
	}
	else if (_cmd->argline == 1) { // seconde line,  theme to remove
		int istart = 0;
		if ((istart = str_next(line, istart, cmd->theme, MAX_THEME_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		 
		// check line termination
		if (!check_line_termination(line, istart)) 
			return -BAD_COMMAND_ARGS; 
		_cmd->argline++;
		return 1;
	}
	if (check_line_termination(line, 0)) return 0;
	return -BAD_COMMAND;
}

static int args_remove_topic(Cmd * _cmd, const char *line) {
 	CmdCreateTopic *cmd = (CmdCreateTopic *) _cmd;
	if (_cmd->argline == 0) { // first line, authentication args
		if ((_cmd->user=getuser(line)) == NULL) return -BAD_USER_AUTH;
		_cmd->argline++;
		return 1;
	}
	else if (_cmd->argline == 1) { // second line, new topic from theme
		int istart = 0;
		if ((istart = str_next(line, istart, cmd->theme, MAX_THEME_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		if ((istart = str_next(line, istart, cmd->topic, MAX_TOPIC_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		// check line termination
		if (!check_line_termination(line, istart)) 
			return -BAD_COMMAND_ARGS; 
		_cmd->argline++;
		return 1;
	}
	if (check_line_termination(line, 0)) return 0;
	return -BAD_COMMAND;
}

static int args_join_topic(Cmd * _cmd, const char *line) {
 	CmdJoinTopic *cmd = (CmdJoinTopic *) _cmd;
	if (_cmd->argline == 0) { // first line, authentication args
		if ((_cmd->user=getuser(line)) == NULL) return -BAD_USER_AUTH;
		_cmd->argline++;
		return 1;
	}
	else if (_cmd->argline == 1) { // second line, topic to join in theme
		int istart = 0;
		if ((istart = str_next(line, istart, cmd->theme, MAX_THEME_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		if ((istart = str_next(line, istart, cmd->topic, MAX_TOPIC_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		// check line termination
		if (!check_line_termination(line, istart))
			return -BAD_COMMAND_ARGS; 
		_cmd->argline++;
		return 1;
	}
	else if(_cmd->argline == 2)  { // third line, msg port
		int istart=0;
		char word[7];
		if ((istart = str_next(line, istart, word, 7)) < 0 ||
			(cmd->port = atoi(word)) <= 0) 
			return -BAD_COMMAND_ARGS;
		// check line termination
		if (!check_line_termination(line, istart)) 
			return -BAD_COMMAND_ARGS; 
		_cmd->argline++;
		return 1;
	}
	if (check_line_termination(line, 0)) return 0;
	return -BAD_COMMAND;
}

static int args_leave_topic(Cmd * _cmd, const char *line) {
 	CmdLeaveTopic *cmd = (CmdLeaveTopic *) _cmd;
	if (_cmd->argline == 0) { // first line, authentication args
		if ((_cmd->user=getuser(line)) == NULL) return -BAD_USER_AUTH;
		_cmd->argline++;
		return 1;
	}
	if (_cmd->argline == 1) { // second line, topic to join in theme
		int istart = 0;
		if ((istart = str_next(line, istart, cmd->theme, MAX_THEME_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		if ((istart = str_next(line, istart, cmd->topic, MAX_TOPIC_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		// check line termination
		if (!check_line_termination(line, istart)) 
			return -BAD_COMMAND_ARGS; 
		_cmd->argline++;
		return 1;
	}
	
	if (check_line_termination(line, 0)) return 0;
	return -BAD_COMMAND;
}

static int args_broadcast(Cmd * _cmd, const char *line) {
	CmdBroadcast *cmd = (CmdBroadcast *) _cmd;
	if (_cmd->argline == 0) { // first line, authentication args
		if ((_cmd->user=getuser(line)) == NULL) return -BAD_USER_AUTH;
		_cmd->argline++;
		return 1;
	}
	if (_cmd->argline == 1) { // second line, topic to broadcast in theme
		int istart = 0;
		if ((istart = str_next(line, istart, cmd->theme, MAX_THEME_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		if ((istart = str_next(line, istart, cmd->topic, MAX_TOPIC_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		// check line termination
		if (!check_line_termination(line, istart)) 
			return -BAD_COMMAND_ARGS; 
		_cmd->argline++;
		return 1;
	}
	if (_cmd->argline == 2) { // third line, first broadcast line
		cmd->nlines = 0;
		cmd->sz_lines[cmd->nlines] = _cmd->chn->line_pos;
		cmd->lines[cmd->nlines++] = strdup(line);
		_cmd->argline++;
		return 1;
	}
	if (check_line_termination(line, 0)) return 0;
	cmd->sz_lines[cmd->nlines] = _cmd->chn->line_pos;
	cmd->lines[cmd->nlines++] = strdup(line);
	_cmd->argline++;
	return 1;
}

static int args_msg(Cmd * _cmd, const char *line) {
	CmdMsg *cmd = (CmdMsg *) _cmd;
	if (_cmd->argline == 0) { // first line, authentication args
		if ((_cmd->user=getuser(line)) == NULL) return -BAD_USER_AUTH;
		_cmd->argline++;
		return 1;
	}
	if (_cmd->argline == 1) { // second line, topic to message in theme
		int istart = 0;
		if ((istart = str_next(line, istart, cmd->theme, MAX_THEME_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		if ((istart = str_next(line, istart, cmd->topic, MAX_TOPIC_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		if ((istart = str_next(line, istart, cmd->user_dest, MAX_USER_NAME)) < 0)
			return -BAD_COMMAND_ARGS;
		// check line termination
		if (!check_line_termination(line, istart)) 
			return -BAD_COMMAND_ARGS; 
		_cmd->argline++;
		return 1;
	}
	if (_cmd->argline == 2) { // third line, first broadcast line
		cmd->nlines = 0;
		cmd->sz_lines[cmd->nlines] = _cmd->chn->line_pos;
		cmd->lines[cmd->nlines++] = strdup(line);
		_cmd->argline++;
		return 1;
	}
	if (check_line_termination(line, 0)) return 0;
	cmd->sz_lines[cmd->nlines] = _cmd->chn->line_pos;
	cmd->lines[cmd->nlines++] = strdup(line);
	_cmd->argline++;
	return 1;
}

static int args_stop(Cmd * _cmd, const char *line) {
  
	if (check_line_termination(line, 0)) return 0;
	return -BAD_COMMAND;
}

ArgsReader cmd_args_reader(Cmd *cmd) {
	switch(cmd->type) {
		case Regist:
			return args_regist;
		case Unregist:
			return args_unregist;
		case CreateTheme:
			return args_create_theme;
		case CreateTopic:
			return args_create_topic;
		case ListThemes:
			return args_empty;
		case ListTopics:
			return args_list_topics;
		case RemoveTheme:
			return args_remove_theme;
		case RemoveTopic:
			return args_remove_topic;
		case JoinTopic:
			return args_join_topic;
		case LeaveTopic:
			return args_leave_topic;
		case Broadcast:
			return args_broadcast;
		case Message:
			return args_msg;
		case ListUsers:
			return args_list_users;
		case Stop:
			return args_stop;
		default:
			return args_bad;
	}
}


/*
 * cmommand creator
 */

Cmd *cmd_create(const char *cmd_name, channel_t *chn) {
	Cmd *cmd = NULL;
	if (strcasecmp("REGIST\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(CmdRegist)); 
		cmd->type = Regist;
	}
	else if (strcasecmp("UNREGIST\n", cmd_name) == 0) {		
		cmd =  (Cmd *) calloc(1, sizeof(CmdUnregist)); 
		cmd->type = Unregist;
	}
	else if (strcasecmp("CREATE_THEME\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(CmdCreateTheme)); 
		cmd->type = CreateTheme;
	}
	else if (strcasecmp("CREATE_TOPIC\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(CmdCreateTopic)); 
		cmd->type = CreateTopic;
	}
	else if (strcasecmp("LIST_THEMES\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(CmdListThemes)); 
		cmd->type = ListThemes;
	}
	else if (strcasecmp("LIST_TOPICS\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(CmdListTopics)); 
		cmd->type = ListTopics;
	}
	else if (strcasecmp("REMOVE_THEME\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(CmdRemoveTheme)); 
		cmd->type = RemoveTheme;
	}
	else if (strcasecmp("REMOVE_TOPIC\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(CmdRemoveTopic)); 
		cmd->type = RemoveTopic;
	}
	else if (strcasecmp("JOIN_TOPIC\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(CmdJoinTopic)); 
		cmd->type = JoinTopic;
	}
	else if (strcasecmp("LEAVE_TOPIC\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(CmdLeaveTopic)); 
		cmd->type = LeaveTopic;
	}
	else if (strcasecmp("BROADCAST\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(CmdBroadcast)); 
		cmd->type = Broadcast;
	}
	else if (strcasecmp("MESSAGE\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(CmdMsg)); 
		cmd->type = Message;
	}
	else if (strcasecmp("STOP\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(Cmd)); 
		cmd->type = Stop;
	}
	else if (strcasecmp("LIST_USERS\n", cmd_name) == 0) {
		cmd =  (Cmd *) calloc(1, sizeof(Cmd)); 
		cmd->type = ListUsers;
	}
	if (cmd != NULL) {
		cmd->chn = chn;
		printf("malloc: create command\n"); inc_mallocs();
	}
	return cmd;
}
				
void cmd_destroy(Cmd *cmd) {
	if (cmd != NULL) {
		printf("free: cmd_destroy\n"); inc_frees();
		free(cmd);
	}
}


/*
 * registration  command
 */
void cmd_reg(Cmd * _cmd) {
	int status = STATUS_OK;
	if (_cmd->type != Regist) status = BAD_COMMAND;
	else {
		CmdRegist *cmd = (CmdRegist*) _cmd;
		int res = user_create(cmd->username, cmd->passwd, cmd->number);
		if (res != OPER_OK)
			status = COMMAND_ERROR + res;
			
	}
	send_status_response(_cmd->chn, status);
}

/*
 * unregistration  command
 */
void cmd_unreg(Cmd *_cmd) {
	int status = STATUS_OK;
	if (_cmd->type != Unregist) status = BAD_COMMAND;  
	else {
		int res = user_remove(_cmd->user);
		if (res != OPER_OK)
			status = COMMAND_ERROR + res;
	}
	send_status_response(_cmd->chn, status);
}

/*
 * Create theme command
 */
void cmd_create_theme(Cmd *_cmd) {
	int status =  STATUS_OK;
	if (_cmd->type != CreateTheme) status = BAD_COMMAND; 
	else {
		CmdCreateTheme *cmd = (CmdCreateTheme *) _cmd;
		int res = theme_create(cmd->theme, _cmd->user);
		if (res != OPER_OK)
			status = COMMAND_ERROR + res;
	}
	send_status_response(_cmd->chn, status);
}

/*
 * Create topic command
 */
void cmd_create_topic(Cmd *_cmd) {
	if (_cmd->argline != 3) {
		send_status_response(_cmd->chn, BAD_COMMAND_ARGS);
		return;
	}
	int status = STATUS_OK;
	if (_cmd->type != CreateTopic) status = BAD_COMMAND;  
	else {
		CmdCreateTopic *cmd = (CmdCreateTopic *) _cmd;
		struct sockaddr_in addr;
		int addr_len;
		uv_tcp_getpeername((uv_tcp_t*) _cmd->chn, (struct sockaddr* ) &addr, &addr_len);
		addr.sin_port = cmd->port;
		int res = topic_create(cmd->theme, cmd->topic, &addr, _cmd->user);
		if (res != OPER_OK)
			status = COMMAND_ERROR + res;
	}
	send_status_response(_cmd->chn, status);
}

/*
 * list themes command
 */
void cmd_list_themes(Cmd *_cmd) {

	if (_cmd->type != ListThemes) send_status_response(_cmd->chn, BAD_COMMAND);  
	else {
		names_result_t names;
		int res = themes_collection(&names);

		if (res != OPER_OK)
			send_status_response(_cmd->chn,  COMMAND_ERROR + res);
		else {
			Answer a = {.status = STATUS_OK };
			a.names = names;
			send_response(_cmd->chn, &a);
		}
	}
	 
}


/*
 * lLst topics command
 */
void cmd_list_topics(Cmd *_cmd) {
	
	if (_cmd->type != ListTopics ) send_status_response(_cmd->chn, BAD_COMMAND); 
	else {
		CmdListTopics *cmd = (CmdListTopics *) _cmd;
		names_result_t names;
		int res = topics_collection(cmd->theme, &names);

		if (res != OPER_OK)
			send_status_response(_cmd->chn,  COMMAND_ERROR + res);
		else {
			Answer a = {.status = STATUS_OK };
			a.names = names;
			send_response(_cmd->chn, &a);
		}
	}
}


/*
 * lLst users command
 */
void cmd_list_users(Cmd *_cmd) {
	
	if (_cmd->type != ListUsers ) send_status_response(_cmd->chn, BAD_COMMAND); 
	else {
	 
		names_result_t names;
		int res = users_collection(&names);

		if (res != OPER_OK)
			send_status_response(_cmd->chn,  COMMAND_ERROR + res);
		else {
			Answer a = {.status = STATUS_OK };
			a.names = names;
			send_response(_cmd->chn, &a);
		}
	}
}


/*
 * remove theme command
 */
void cmd_remove_theme(Cmd *_cmd) {
	int status = STATUS_OK;
	if (_cmd->type != RemoveTheme) status = BAD_COMMAND; 
	else {
		CmdRemoveTheme *cmd = (CmdRemoveTheme *) _cmd;
		
		int res = theme_remove(cmd->theme, _cmd->user);

		if (res != OPER_OK)
			status = COMMAND_ERROR + res;	 
	}
	send_status_response(_cmd->chn, status);
}


/*
 * remove topic command
 */
void cmd_remove_topic(Cmd *_cmd) {
	int status = STATUS_OK;
	if (_cmd->type != RemoveTopic) status = BAD_COMMAND; 
	else {
		CmdRemoveTopic *cmd = (CmdRemoveTopic *) _cmd;
		int res = topic_remove(cmd->theme, cmd->topic, _cmd->user);
		if (res != OPER_OK)
			status = COMMAND_ERROR + res;
	}
	send_status_response(_cmd->chn, status);
}




typedef struct udp_send {
	uv_udp_send_t req;
	Cmd *cmd;
	char buffer[1];
} udp_send_t;


void enter_partner_cb(uv_udp_send_t *req , int status) {
	udp_send_t *send = (udp_send_t *) req;
	 
	// check send error
	if (status != 0) {
		send_status_response(send->cmd->chn, UNREACHABLE_TOPIC_OWNER);
		printf("free: enter_partner_cb\n"); inc_frees();
		free(send);
	}
	
	CmdJoinTopic *cmd = (CmdJoinTopic*) send->cmd;
	Answer a = {.status = STATUS_OK };
	a.njoiners = cmd->njoiners;
	send_response(cmd->base.chn, &a);
	
	printf("free: enter_partner_cb\n"); inc_frees();
	free(send);
}


/*
 * join topic command
 * int uv_tcp_getpeername(const uv_tcp_t* handle, struct sockaddr* name, int* namelen)
 * send to topic owner a msg announcing the new partner
 */
void cmd_join_topic(Cmd *_cmd) {
	
	if (_cmd->type != JoinTopic) send_status_response(_cmd->chn, BAD_COMMAND);
	else {
		CmdJoinTopic *cmd = (CmdJoinTopic *) _cmd;
		struct sockaddr_in addr;
		int addr_len;
		int njoiners;
		uv_tcp_getpeername((uv_tcp_t*) _cmd->chn, (struct sockaddr* ) &addr, &addr_len);
		addr.sin_port = cmd->port;
		int res = topic_join(cmd->theme, cmd->topic, &addr, _cmd->user, &njoiners);
		if (res != OPER_OK) send_status_response(_cmd->chn, COMMAND_ERROR + res);
		else {
		
			joiner_info_t owner_info;
			
			if ((res = topic_owner_info(cmd->theme, cmd->topic, &owner_info) ) != OPER_OK) 
				send_status_response(_cmd->chn, COMMAND_ERROR + res);
			else {
				cmd->njoiners = njoiners;
				
				printf("malloc: udp_send_t for join_topic\n"); inc_mallocs();
				
				printf("Owner info -> %s;%X:%d\n", owner_info.username,
							owner_info.sock_addr.sin_addr.s_addr, owner_info.sock_addr.sin_port);
				udp_send_t *send = (udp_send_t *) malloc(sizeof(uv_udp_send_t)+128);
				// prepare notification message
				int msg_size = sprintf(send->buffer, "ENTER_PARTNER\n%s %s %d\n\n", 
					cmd->topic, _cmd->user->name, njoiners);
			 
				send->cmd = _cmd;
				uv_buf_t buf;
				
				buf.base = send->buffer;
				buf.len = msg_size;
				uv_udp_send(&send->req, &broadcaster, &buf, 1, (struct sockaddr *)  &owner_info.sock_addr, enter_partner_cb);
			}
		}
			
	}
}

void leave_partner_cb(uv_udp_send_t *req , int status) {
	udp_send_t *send = (udp_send_t *) req;
	 
	// check send error
	if (status != 0) {
		send_status_response(send->cmd->chn, UNREACHABLE_TOPIC_OWNER);
		printf("free: leave_partner_cb\n"); inc_frees();
		free(send);
		return;
	}
	
	/* free message resources */
	
	CmdLeaveTopic *cmd = (CmdLeaveTopic*) send->cmd;
	Answer a = {.status = STATUS_OK };
	a.njoiners = cmd->njoiners;
	send_response(cmd->base.chn, &a);
	printf("free: leave_partner_cb\n"); inc_frees();
	free(send);
}


/*
 * leave topic command
 */
void cmd_leave_topic(Cmd *_cmd) {
	if (_cmd->type != LeaveTopic) send_status_response(_cmd->chn, BAD_COMMAND);
	else {
		CmdLeaveTopic *cmd = (CmdLeaveTopic *) _cmd;
		int njoiners;
		int res = topic_leave(cmd->theme, cmd->topic, _cmd->user, &njoiners);
		if (res != OPER_OK) send_status_response(_cmd->chn, COMMAND_ERROR + res);
		else {
			joiner_info_t owner_info;
			
			if ((res = topic_owner_info(cmd->theme, cmd->topic, &owner_info) ) != OPER_OK) 
				send_status_response(_cmd->chn, COMMAND_ERROR + res);
			else {
				cmd->njoiners = njoiners;
				printf("malloc: udp_send_t for leave_topic\n"); inc_mallocs();
				printf("Owner info -> %s;%X:%d\n", owner_info.username,
							owner_info.sock_addr.sin_addr.s_addr, owner_info.sock_addr.sin_port);
				udp_send_t *send = (udp_send_t *) malloc(sizeof(uv_udp_send_t)+128);
				int msg_size = sprintf(send->buffer, "LEAVE_PARTNER\n%s %s %d\n\n", 
					cmd->topic, _cmd->user->name, njoiners);
				 
				send->cmd = _cmd;
				uv_buf_t buf;
				
				buf.base = send->buffer;
				buf.len = msg_size;
				uv_udp_send(&send->req, &broadcaster, &buf,1, (struct sockaddr *)  &owner_info.sock_addr, leave_partner_cb);
			}
		}
			
	}
	
}


void send_cb(uv_udp_send_t *req, int status) {
	CmdBroadcast *cmd = (CmdBroadcast *) req->data;
	cmd->send_partners++;
	if (cmd->send_partners == cmd->total_partners) {
		send_status_response(cmd->base.chn, STATUS_OK);
		req_array_entry_t *entry = (req_array_entry_t*) req;
		printf("free: broadcast_cb\n"); inc_frees();
		free(entry- entry->idx);
		
		
	}
}

void send_msg_cb(uv_udp_send_t *req, int status) {
	Cmd *cmd = (Cmd*) req->data;
	send_status_response(cmd->chn, STATUS_OK);
 
}




void send_to_partners(CmdBroadcast *cmd, joiner_info_t *joiners, int total) {
	// TODO no send the message to the emmiter
	printf("malloc: udp_send_t array for broadcast\n"); inc_mallocs();
	cmd->reqs = (req_array_entry_t*) malloc(sizeof(req_array_entry_t)*total);
	for (int i=0; i < total; ++i) {
		cmd->reqs[i].req.data = cmd;
	}	
	cmd->total_partners = total;
	cmd->send_partners = 0;
	int info_size = sprintf(cmd->send_info, "%s %s %s %d\n", 
		cmd->base.user->name, cmd->theme, cmd->topic, total);
	// prepare message to partner p
	uv_buf_t bufs[cmd->nlines+1];
	bufs[0].base = cmd->send_info;
	bufs[0].len = info_size;
	for (int l=1; l <= cmd->nlines; ++l) {
		bufs[l].base = cmd->lines[l-1];
		bufs[l].len = cmd->sz_lines[l-1];
	}
	for (int p = 0; p < total; p++) {
		cmd->reqs[p].idx = p;
		uv_udp_send(&cmd->reqs[p].req, &broadcaster, bufs, cmd->nlines + 1, (struct sockaddr *) &joiners[p].sock_addr, send_cb);
	}
	
}


static void topic_broadcast(CmdBroadcast *cmd, user_t *user) {
	joiner_info_t *joiners = NULL;
	int total = 0;
	int res;
	if ( (res = topic_joiners(cmd->theme, cmd->topic, &joiners, &total, user)) != OPER_OK)
		send_status_response(cmd->base.chn, COMMAND_ERROR + res);
	else {
		// now send the message to all the topic partners
		send_to_partners(cmd, joiners, total);
	}
	
}

static void send_message(CmdMsg *cmd) {
	user_t *sender = cmd->base.user;
	
	int res;
	joiner_info_t dest_info ;
	if ((res=topic_joiner_search(cmd->theme, cmd->topic, sender, cmd->user_dest, &dest_info ) ) != OPER_OK) {
		send_status_response(cmd->base.chn, COMMAND_ERROR + res);
	}	
	else {
		// now send the message to all the topic partners
	  
		int info_size = sprintf(cmd->send_info, "%s %s %s %s\n", 
				cmd->base.user->name, cmd->theme, cmd->topic, cmd->user_dest);
		// prepare message to partner p
		uv_buf_t bufs[cmd->nlines+1];
		bufs[0].base = cmd->send_info;
		bufs[0].len = info_size;
		for (int l=1; l <= cmd->nlines; ++l) {
			bufs[l].base = cmd->lines[l-1];
			bufs[l].len = cmd->sz_lines[l-1];
		}
		cmd->req.data = cmd;
		uv_udp_send(&cmd->req, &broadcaster, bufs, cmd->nlines + 1, (struct sockaddr *)  &dest_info.sock_addr, send_msg_cb);
	 
	}
	
}

/*
 * broadcast command
 */
void cmd_broadcast(Cmd *_cmd) {
	 
	if (_cmd->type != Broadcast) send_status_response(_cmd->chn, BAD_COMMAND);
	CmdBroadcast *cmd = (CmdBroadcast *) _cmd;
	 
	 
	topic_broadcast(cmd, _cmd->user);
}	 

/*
 * Stop server
 */

void cmd_stop(Cmd *cmd) {
	stop_server();
	send_status_response(cmd->chn, STATUS_OK);
}

/*
 * send a unicast message
 */
void cmd_message(Cmd *_cmd) {
	 
	if (_cmd->type != Message) send_status_response(_cmd->chn, BAD_COMMAND);
	CmdMsg *cmd = (CmdMsg *) _cmd;
	 
	 
	send_message(cmd);
	 
}



void cmd_exec(Cmd *cmd) {
	 
	switch(cmd->type) {
		case Regist:
			cmd_reg(cmd);	break;
		case Unregist:
			cmd_unreg(cmd); break;
		case CreateTheme:
			cmd_create_theme(cmd); break;
		case CreateTopic:
			cmd_create_topic(cmd); break;
		case ListThemes:
			cmd_list_themes(cmd); break;
		case ListTopics:
			cmd_list_topics(cmd); break;
		case RemoveTheme:
			cmd_remove_theme(cmd); break;
		case JoinTopic:
			cmd_join_topic(cmd); break;
		case LeaveTopic:
			cmd_leave_topic(cmd); break;
		case Broadcast:
			cmd_broadcast(cmd); break;
		case RemoveTopic:
			cmd_remove_topic(cmd); break;
		case Message:
			cmd_message(cmd); break;
		case ListUsers:
			cmd_list_users(cmd); break;
		case Stop:
			cmd_stop(cmd); break;
		default: 
			break;
		
	}
}	
