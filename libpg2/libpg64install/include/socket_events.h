/*------------------------------------------------------------
 * Define a simple, aysnchronous event driven API 
 * allowing use of sockets communication in libgraph applications
 * 
 * ISEL, 2020
 *----------------------------------------------------------*/

#pragma once

#include "events.h"

#include "uv_srv.h"


#define CREATE_GAME "CREATE_TOPIC"
#define JOIN_GAME   "JOIN_TOPIC"
#define PLAY		"BROADCAST"


#ifdef __cplusplus 
extern "C" {
#endif

// command to connect to a group server 
// for future requests
// on a connection a list battleship games command is send
session_t gs_connect(const char gs_ip_addr[], 
					 const char user[],
					 ResponseEventHandler on_response,
					 MsgEventHandler on_msg);
 
							

/*
 * A request/response async pattern with tcp sockets for group server
 */
void gs_request(session_t session, const char cmd[], const char args[]);



// simplified api for group server communication
int session_get_msg_port(session_t);

void  gs_end(session_t session);


void ge_get_games(session_t session);


void gs_create_game(session_t session, char *name, int max_opponents);


void gs_join_game(session_t session, char *name);


// just the owner of the game invoke the this
void gs_start_game(session_t session, char *opponnents_order);

void gs_end_game(session_t session);

void gs_play(session_t session);  


/*
 * A request/response async pattern with tcp sockets
 */
void sock_request(const char ip_addr[], 
					int ip_port, 
					const char cmd[],
					ResponseEventHandler on_response);

/*
 * Invite a peer to create a virtual communication channel
 */
void peer_invite(const char ip_addr[], int ip_port, PeerEventHandler on_answer);

/*
 * accept a peer to create a virtual communication channel
 */
void peer_accept(PeerEventHandler on_invite);


/*
 * send to peer.
 * For now, it is a synchronous operation, 
 * If we face problems, we have to turn it asynchronous, too!
 */

int  peer_sendto(PeerSock other, char msg[]);



/*
 * subscribe a stream of peer messages
 */
void start_receive(PeerSock other, ReadEventHandler on_read);

/*
 * peer_end.
 * For now, it corersponds just to the undelying socket close
 */
void peer_end(PeerSock peer);
				  


void send_graph_response(msg_request_t *msg);
void send_graph_notification(session_t session);
 
#ifdef __cplusplus 
}
#endif
