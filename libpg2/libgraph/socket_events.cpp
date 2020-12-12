
/*------------------------------------------------------------
 * Define a simple, aysnchronous event driven API 
 * allowing use of sockets communication in libgraph applications
 * 
 * ISEL, 2020
 *----------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include "unistd.h"

#include "../include/graphics.h"
#include "../include/socket_events.h"

#define GROUP_SERVER_PORT	60000


//#define DEBUG_CMDS
//#define DEBUG_END
//#define DEBUG_EVENTS

// last received message number
static unsigned long msg_number = 0;


// generic creation of a message request to libuv
static msg_request_t *mr_create(msg_type_t type, const char cmd[], ResponseEventHandlerEx on_response)  {
	
	msg_request_t *msg = (msg_request_t *) calloc(1, sizeof(msg_request_t));
	msg->type = type;
	if (cmd != NULL) msg->cmd = strdup(cmd);
	else msg->cmd = NULL;
	msg->on_response = on_response;
	msg->resp_type = Response;
	msg->number = ++msg_number;
	return msg;
}

// specialized mesage request creator
static msg_request_t *mr_gs_cmd(session_t session, const char cmd[])  {
	if (session->state != Connected) {
		session->on_response(-4, "Not connected session!", session->context);
		return NULL;
	}
	if (session->chn != NULL && session->chn->busy) {
		session->on_response(-4, "channel is busy!", session->context);
		return NULL;
	}
	msg_request_t *mr=  mr_create(GroupSrvCmd, cmd, session->on_response);
#ifdef DEBUG_CMDS
	printf("cmd send: '%s'\n", cmd);
#endif
	mr->session = session;
	
	return mr;
}

// specialized message request creator for group server connection
msg_request_t *mr_gs_connect(session_t session)  {
 	
	if (session->state != Created) {
		session->on_response(-4, "Already created session!", session->context);
		return NULL;
	}
	session->state = Pending;
	

	msg_request_t *mr=  mr_create(GroupSrvConnect, NULL, session->on_response);
	strcpy(mr->ip_addr, session->sip);
	mr->ip_port = GROUP_SERVER_PORT;
	mr->session = session;

	return mr;
}

// specialized message request creator for a generic server request
static msg_request_t *mr_generic_srv(const char ip_addr[], int ip_port,
							const char cmd[], ResponseEventHandlerEx on_response)  {
	msg_request_t *msg = mr_create(GenericRequest, cmd, on_response);
	strcpy(msg->ip_addr, ip_addr);
	msg->ip_port = ip_port;
	return msg;
	
}

// creator of a sesssion object representing an active connection with a group server
static session_t gs_session_create(const char gs_addr[], 
						const char user[],
						ResponseEventHandlerEx on_response,
						MsgEventHandlerEx on_msg,
						void *ctx) {
	session_t session = (session_t) malloc(sizeof(sess_t));
	char pass[64];
	
	sprintf(pass, "%s_pass", user);
	strcpy(session->sip, gs_addr);
	strcpy(session->user, user);
	strcpy(session->pass, pass);
	session->on_response = on_response;
	session->on_msg = on_msg;
	session->state = Created;
	session->context = NULL;
	//no sockets for now
	session->chn = NULL;
	session->msg_port = 0;
	session->context = ctx;
	
	// stuff for partners message thread-safe access
	pthread_mutex_init(&session->lock, NULL);
	pthread_cond_init(&session->empty, NULL);
	 
	session->notification = NULL;
	return session;
	
}

void gs_session_destroy(session_t session) {
	
	if (session->chn == NULL) return;
	msg_request_t *mr = mr_create(CloseChannel, NULL, session->on_response);
	
	if (!try_close_session(session)) return;
	session->state = Closing;
	mr->session=session;
#ifdef DEBUG_END
	printf("exec session destroy for %s!\n", session->user);
#endif
	exec_request(mr);
}
					

// command to connect to a group server 
// for future requests
// on a connection a list battleship games command is send
session_t gs_connect(const char gs_addr[], 
					 const char user[],
					 ResponseEventHandlerEx on_response,
					 MsgEventHandlerEx on_msg,
					 void *ctx) {
	
	
	session_t session = gs_session_create(gs_addr, user, on_response, on_msg, ctx);
	msg_request_t *mr = mr_gs_connect(session);
	exec_request(mr);
	return session;
}
							

/*
 * A request/response async pattern with tcp sockets for group server
 */
void gs_request(session_t session, const char cmd[], const char args[]) {
	char authcmd[1024];
	
	if (session->state >= Closing) {
#ifdef DEBUG_END
			printf("try send cmd with session state %d: '%s'\n", session->state, cmd);
#endif
			session->on_response(-5, "Session closing or closed!", session->context);
			return;
	}
	int auth_size = sprintf(authcmd, "%s\n%s %s\n", cmd, session->user, session->pass);
	int args_size = strlen(args) ;
	if (session->chn == NULL)
		session->on_response(-5, "Channel closed", session->context);
	else if (session->chn != NULL && !session->chn->valid)
		session->on_response(-4, "Invalid session channel", session->context);
	else if (args_size+auth_size >=1023) // cmd too big, tell error via callback
		session->on_response(-1, "Command too big", session->context);
	else if (args_size<2 || args[args_size-1] != '\n' || args[args_size-2] != '\n')
		session->on_response(-2, "Bad terminated command", session->context);
	else {
		sprintf(authcmd+auth_size, "%s", args);
		msg_request_t *msg = mr_gs_cmd(session, authcmd);
		 
		exec_request(msg);
	}
}


/*
 * A generic request/response async pattern with tcp sockets
 */
void tcp_request(const char ip_addr[], 
					int ip_port, 
					const char cmd[],
					ResponseEventHandlerEx on_response) {
	msg_request_t *msg = mr_generic_srv(ip_addr,ip_port,cmd, on_response);
	exec_request(msg);
}


int session_get_msg_port(session_t session) {
	return session->msg_port;
}

 
				  


//
// Communication with libuv

// monitor for rendez-vouz session group partners messages


void save_notification(session_t session) {
	pthread_mutex_lock(&session->lock);
		
	char *msg = strdup(session->notification_buffer);
 
	while (session->notification != NULL)
		pthread_cond_wait(&session->empty, &session->lock);
	session->notification = msg;
	pthread_mutex_unlock(&session->lock);
}

char* get_notification(session_t session) {
	pthread_mutex_lock(&session->lock);
	char *msg = session->notification;
	session->notification = NULL;
	pthread_mutex_unlock(&session->lock);
	pthread_cond_signal(&session->empty); 
	return msg;
}


bool try_close_session(session_t session) {
	pthread_mutex_lock(&session->lock);
	bool success;
	 
	if ((success = (session->state < Closing))) 
		session->state = Closing;
	pthread_mutex_unlock(&session->lock);
	return success;
	 
}

void send_graph_response(msg_request_t *msg) {
	SDL_Event event;
    
    SDL_memset(&event, 0, sizeof(event));
    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */

	 
	event.user.code = REQUEST_RESPONSE_EVENT;
	
    event.user.type = _RESPONSE_EVENT;
   
    event.user.data1 = msg;
    event.user.data2 = NULL;

    event.type = _RESPONSE_EVENT;
    
    while (0 >= SDL_PushEvent(&event)) {
#ifdef DEBUG_EVENTS
		printf("error pushing event!\n");
#endif
		SDL_Delay(2);
	}
}


void send_graph_notification(session_t session) {
	SDL_Event event;
	
    SDL_memset(&event, 0, sizeof(event));
	event.user.code = NOTIFICATION_EVENT;
	 	
    event.user.type = _NOTIFICATION_EVENT;
   
    event.user.data1 = session;
    event.user.data2 = NULL;

    event.type = _NOTIFICATION_EVENT;
    
   
    while (0 >= SDL_PushEvent(&event)) {
#ifdef DEBUG_EVENTS
		printf("error pushing event!\n");
#endif
		SDL_Delay(2);
	}
} 
