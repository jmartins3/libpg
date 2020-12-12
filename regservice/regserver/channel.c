#include <stdio.h>

 
#include <stdlib.h>

#include <ctype.h>
#include <string.h>


#include "channel.h"
#include  "commands.h"
#include "responses.h"
#include "strutils.h"
#include "activeusers.h"


#define DEBUG_CMDS
#define DEBUG_MEM


extern bool verbose_mode;

/*
 * channel related functions
 */
 

static inline void chn_clear_line(channel_t * c) {
	c->line_pos=0;
}

static inline void chn_init_buffer(channel_t * c, int length) {
	c->rw_pos=0;
	c->len=length;
}

static inline void chn_clear_buf(channel_t * c) {
	chn_init_buffer(c,0);
}

 
/* try to complete a line from the connection buffer
 * Called on the read completed callback
 * the returned line include the line terminator
 */
static bool chn_trygetline(channel_t *chn) {
	char c;
	
	while (chn->rw_pos < chn->len && (c= chn->buffer[chn->rw_pos++]) != LF) {
		if (chn->line_pos < MAX_LINE-1) 
			if ( c!= CR) chn->line[chn->line_pos++] = c;
	}
	chn->line[chn->line_pos++] = LF;
	chn->line[chn->line_pos] = 0;
	return c == LF;	
}


/* try to complete a line from the connection buffer
 * Called on the read completed callback
 * the returned line include the line terminator
 * Additionaly clear the field that indicates the insertion position in line
 */
static bool chn_trygetline2(channel_t *chn) {
	if (!chn_trygetline(chn)) return false;
	chn_clear_line(chn);
	return true;
}

static void synch_error(channel_t *chn) {
	while (chn_trygetline2(chn)) {
		if (check_empty_line(chn->line)) {
			chn->state = GetCommand;
			send_status_response(chn,  chn->error);
			return;
		}
	}
}

/*
 * called on the read completed callback
 */
void chn_process(channel_t *chn, int nread) {
	chn_init_buffer(chn, nread); 	
	
	if (chn->state == Error) {
		synch_error(chn);
		return;
	}
	if (chn->state == GetCommand) {
		if (!chn_trygetline2(chn)) return;
		/* get command from name */
		Cmd *cmd = cmd_create(chn->line, chn);
	 	if (cmd == NULL) {
			chn->state = Error; 
			chn->error= BAD_COMMAND;
			synch_error(chn);
			return;
		}
		else {
			chn->cmd = cmd;
			chn->reader	 = cmd_args_reader(cmd);
			chn->state = GetParms;
		}
#ifdef DEBUG_CMDS
		if (verbose_mode)
			printf("command %s received!\n", chn->line);
#endif
	}
	while (chn->state == GetParms && chn_trygetline(chn) ) {
#ifdef DEBUG_CMDS
		if (verbose_mode)
			printf("args received: %s\n", chn->line);
#endif
		// TODO abort command processing in case of an argument error
		
		if (check_empty_line(chn->line)) {
			chn->state = Execute; 
			chn_clear_line(chn);
			break;
		}
		int res = chn->reader(chn->cmd, chn->line);
		chn_clear_line(chn);
		if (res < 0) {
			chn->state = Error; 
			chn->error = -res;
			synch_error(chn);
			return;
			
		}
		if (res == 0)
			chn->state = Execute; 
		
	}
	
		
	if (chn->state == Execute) {
		// the commands here are treated as synchronous
		// but some may are assynchronous, namely messages to partners commands
		// TODO - refact this
		cmd_exec(chn->cmd);
		chn->state = GetCommand;
	}		
}
	



/*
 * Create a channel that represents de connection to a given client
 * 
 * The channel mantains all the the state necessary for internal management
 * 
 * Parameters: 
 * 		uv_tcp_t server: the listen socket
 */
channel_t * chn_create(uv_loop_t *loop) {
#ifdef DEBUG_MEM
	if (verbose_mode)
		printf("malloc: channel for chn_create\n"); 
	inc_mallocs();
#endif
	channel_t *chn = (channel_t*) malloc(sizeof(channel_t));
	if (chn == NULL) return NULL;
	bzero(chn, sizeof(channel_t));


	uv_tcp_init(loop, (uv_tcp_t *) chn);
  
	chn->state = GetCommand;
	return chn;
	
}

/*
 * Destroy a channel
 * Parameters:
 * 		_chn: the channel (in a uv_handle_t form) to destroy
 */
void chn_destroy(uv_handle_t *_chn) {
	channel_t *chn = (channel_t*)  _chn;
	user_session_t *session = find_session(&chn->socket);
	
	if (session != NULL) 
		session_destroy(session);
#ifdef DEBUG_MEM
	if (verbose_mode)
		printf("free: channel\n"); 
	inc_frees();
#endif
	free(chn);
}





