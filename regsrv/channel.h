#pragma once

#include <uv.h>
#include "commands.h"




#define BUFFER_SIZE	4096
#define MAX_LINE	512


enum state { GetCommand, Autenthicate, GetParms, Execute, Error, Completed };
 


// this should be enough for http echo server
typedef struct channel {
	uv_tcp_t socket;		 // the channel socket
    uv_write_t writereq;	 // Write request - must survive until write completion	
	char buffer[BUFFER_SIZE];// I/O buffer 
	int rw_pos;              // current I/O buffer position 
	int len;                 // buffer data length 
    char line[MAX_LINE+1];	 // line receiver
    int line_pos;			 // line current position
	enum state state;		 // state for machine state
	int error;
	// the next field is not used since we are using parms line number
	// to identify the state
	int get_parms_state;	 // state on reading parms
	// current command stuf
	Cmd *cmd;				 // current command
	char *user;				 // user id
	ArgsReader reader;		 // command args reader
} channel_t;


void chn_process(channel_t *chn, int nread);



/*
 * Create a channel that represents de connection to a given client
 * 
 * The channel mantains all the the state necessary for internal management
 * 
 * Parameters: 
 * 		uv_tcp_t server: the listen socket
 */
channel_t * chn_create(uv_loop_t *loop);


/*
 * Destroy a channel
 * Parameters:
 * 		_chn: the channel (in a uv_handle_t form) to destroy
 */
void chn_destroy(uv_handle_t *_chn);


/*
 * called on the read completed callback
 */
void chn_process(channel_t *chn, int nread);

void stop_server();
