/*------------------------------------------------------------
 * Define a simple, aysnchronous event driven API 
 * allowing use of sockets communication in libgraph applications
 * 
 * ISEL, 2020
 *----------------------------------------------------------*/

#pragma once

#include "events.h"

#include "uv_srv.h"


#ifdef __cplusplus 
extern "C" {
#endif


extern unsigned int __USER_EVENTS;

#define _RESPONSE_EVENT (__USER_EVENTS)
#define _NOTIFICATION_EVENT (__USER_EVENTS+1)
#define _TIMER_EVENT (__USER_EVENTS+2)
#define _END_LOOP (__USER_EVENTS+3)


// simplified api for group server communication

/**
 *  Command to connect to a group server for future requests
 */
session_t gs_connect(const char gs_ip_addr[], 
					 const char user[],
					 ResponseEventHandlerEx on_response,
					 MsgEventHandlerEx on_msg,
					 void *ctx);
 				

/**
 * A request/response async pattern with tcp sockets for group server
 */
void gs_request(session_t session, const char cmd[], const char args[]);


/**
 * get the session port for asynvhronous messaging receiving
 */
int session_get_msg_port(session_t);

 
/**
 * destroy the given session
 */
void gs_session_destroy(session_t session);

/**
 * A request/response async pattern with tcp sockets
 */
void tcp_request(const char ip_addr[], 
					int ip_port, 
					const char cmd[],
					ResponseEventHandlerEx on_response);


/**
 * Internal function to synchronize the saving
 * of  a received group partner message
 */
void save_notification(session_t session);

/**	
 * Internal function to synchronize the reception of
 * a received group partner message
 */ 
char* get_notification(session_t session);

/**
 * atomically check the state is closing or closed and if  not set to closing
 */
bool try_close_session(session_t session);

/**
 * internal function to send the response to the graph loop
 * for the given request
 */
void send_graph_response(msg_request_t *msg);

/**
 * internal function to send the message to the graph loop
 * from the partner of the given group session
 */
void send_graph_notification(session_t session);
 
#ifdef __cplusplus 
}
#endif
