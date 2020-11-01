/*------------------------------------------------------------
 * Define a simple, aysnchronous event driven API 
 * allowing use of sockets communication in libgraph applications
 * 
 * ISEL, 2020
 *----------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include "../include/graphics.h"
#include "../include/socket_events.h"

#define GROUP_SERVER_PORT	60000

static const char *LIST_BATTLESHIP_GAMES_CMD = "LIST_TOPICS";
static const char *LIST_BATTLESHIP_GAMES_ARGS = "battleship\n";


// generic creation of a message request to libuv
static msg_request_t *mr_create(msg_type_t type, const char cmd[], ResponseEventHandler on_response)  {
	
	msg_request_t *msg = (msg_request_t *) calloc(1, sizeof(msg_request_t));
	msg->type = type;
	msg->cmd = strdup(cmd);
	msg->on_response = on_response;
	msg->resp_type = Response;
	return msg;
}

// specialized mesage request creator
static msg_request_t *mr_gs_cmd(session_t session, const char cmd[])  {
	if (session->state != Connected) {
		session->on_response(-4, "Not connected session!");
		return NULL;
	}
	if (session->chn != NULL && session->chn->busy) {
		session->on_response(-4, "channel is busy!");
		return NULL;
	}
	msg_request_t *mr=  mr_create(GroupSrvCmd, cmd, session->on_response);
	mr->session = session;
	
	return mr;
}

// specialized message request creator for group server connection
msg_request_t *mr_gs_connect(session_t session)  {
	char authcmd[1024];
	
	if (session->state != Created) {
		session->on_response(-4, "Already created session!");
		return NULL;
	}
	session->state = Pending;
	
	sprintf(authcmd, "%s\n%s %s\n%s\n",
		LIST_BATTLESHIP_GAMES_CMD, session->user, session->pass, LIST_BATTLESHIP_GAMES_ARGS);
	msg_request_t *mr=  mr_create(GroupSrvConnect, authcmd, session->on_response);
	strcpy(mr->ip_addr, session->sip);
	mr->ip_port = GROUP_SERVER_PORT;
	mr->session = session;

	return mr;
}

// specialized message request creator for a generic server request
static msg_request_t *mr_generic_srv(const char ip_addr[], int ip_port,
							const char cmd[], ResponseEventHandler on_response)  {
	msg_request_t *msg = mr_create(GenericRequest, cmd, on_response);
	strcpy(msg->ip_addr, ip_addr);
	msg->ip_port = ip_port;
	return msg;
	
}

// creator of a sesssion object representing an active connection with a group server
static session_t gs_session_create(const char gs_addr[], 
						const char user[],
						const char pass[],
						ResponseEventHandler on_response,
						MsgEventHandler on_msg) {
	session_t session = (session_t) malloc(sizeof(sess_t));
	session->chn = NULL;
	strcpy(session->sip, gs_addr);
	strcpy(session->user, user);
	strcpy(session->pass, pass);
	session->on_response = on_response;
	session->on_msg = on_msg;
	session->state = Created;
	// TO DO
	session->msg_port = 2000;
	return session;
	
}
						

// command to connect to a group server 
// for future requests
// on a connection a list battleship games command is send
session_t gs_connect(const char gs_addr[], 
					 const char user[],
					 const char pass[],
					 ResponseEventHandler on_response,
					 MsgEventHandler on_msg) {
	session_t session = gs_session_create(gs_addr, user, pass, on_response, on_msg);
	msg_request_t *mr = mr_gs_connect(session);
	exec_request(mr);
	return session;
}
							

/*
 * A request/response async pattern with tcp sockets for group server
 */
void gs_request(session_t session, const char cmd[], const char args[]) {
	char authcmd[1024];
	
	int auth_size = sprintf(authcmd, "%s\n%s %s\n", cmd, session->user, session->pass);
	int args_size = strlen(args) ;
	if (session->chn != NULL && !session->chn->valid)
		session->on_response(-4, "Invalid session channel");
	else if (args_size+auth_size >=1023) // cmd too big, tell error via callback
		session->on_response(-1, "Command too big");
	else if (args_size<2 || args[args_size-1] != '\n' || args[args_size-2] != '\n')
		session->on_response(-2, "Bad terminated command");
	else {
		sprintf(authcmd+auth_size, "%s", args);
		msg_request_t *msg = mr_gs_cmd(session, authcmd);
		if (session->chn != NULL) session->chn->busy = true;
		exec_request(msg);
	}
}


/*
 * A generic request/response async pattern with tcp sockets
 */
void sock_request(const char ip_addr[], 
					int ip_port, 
					const char cmd[],
					ResponseEventHandler on_response) {
	msg_request_t *msg = mr_generic_srv(ip_addr,ip_port,cmd, on_response);
	exec_request(msg);
}

int session_get_msg_port(session_t session) {
	return session->msg_port;
}

/*
 * Invite a peer to create a virtual communication channel
 */
void peer_invite(const char ip_addr[], int ip_port, PeerEventHandler on_acceptance) {
}

/*
 * accept a peer to create a virtual communication channel
 */
void peer_accept(PeerEventHandler on_invite) {
}


/*
 * send to peer.
 * For now, it is a synchronous operation, 
 * If we face problems, we have to turn it asynchronous, too!
 */

int  peer_sendto(PeerSock other, char msg[]) {
	return 0;
}

 
/*
 * subscribe a stream of peer messages
 */
void start_receive(PeerSock other, ReadEventHandler on_read) {
}


/*
 * peer_end.
 * For now, it corersponds just to the undelying socket close
 */
void peer_end(PeerSock peer) {
}
				  


//
// Communication with libuv



void send_graph_response(msg_request_t *msg) {
	SDL_Event event;
    
    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */

	 
	event.user.code = REQUEST_RESPONSE_EVENT;
	
    event.user.type = SDL_USEREVENT;
   
    event.user.data1 = msg;
    event.user.data2 = NULL;

    event.type = SDL_USEREVENT;
    
    SDL_PushEvent(&event);
}


void send_graph_notification(session_t session) {
	SDL_Event event;
	
	event.user.code = NOTIFICATION_EVENT;
	 	
    event.user.type = SDL_USEREVENT;
   
    event.user.data1 = session;
    event.user.data2 = NULL;

    event.type = SDL_USEREVENT;
    
    SDL_PushEvent(&event);
} 
