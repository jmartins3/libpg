
#pragma once


#include <uv.h>


#define SERVER_PORT 60001
#define DEFAULT_BACKLOG 128
#define BUFFER_SIZE 4096

#define NOTIFICATION_BUFFER_SIZE 4096

#define MAX_IP_ADDR 16
#define MAX_USER_NAME 32
#define MAX_USER_PASS 32

typedef  void (*ResponseEventHandler)(int status, const char response[]);
typedef  void (*MsgEventHandler)(const char sender[], const char msg[]);

struct channel;

typedef enum sess_state { Created, Pending, Connected, SessionClosed } session_state_t;

typedef struct session {
	struct channel*chn;
	uv_udp_t msg_sock;
	int msg_port;
	char notification_buffer[NOTIFICATION_BUFFER_SIZE];
	char *notification;
	char sip[MAX_IP_ADDR];    // group service ip
	char user[MAX_USER_NAME]; // user name
	char pass[MAX_USER_PASS]; // user pass
	session_state_t state;
	
	// callbacks
	ResponseEventHandler on_response;
	MsgEventHandler on_msg;
} sess_t, *session_t;


typedef struct {
	uv_udp_t *chn;			// the underlying uv socket
	char pip[MAX_IP_ADDR];  // peer ip
	int pp;					// peer port
} Peer, *PeerSock;


typedef  void (*PeerEventHandler)(int status, PeerSock other);
typedef  void (*ReadEventHandler)(int status, char response[]);

/*
 * messages for communication with libuv
 */
typedef enum msg_type { GroupSrvConnect, GroupSrvCmd, BroadcastMsg, PrivateMsg, GenericRequest } msg_type_t;
typedef enum response_type { Response, Notification } response_type_t;

typedef struct msg_request {
	// type of message
	msg_type_t type;
	// type of response
	response_type_t resp_type;
	// session in case og group server communication
	session_t session;
	// request
	char ip_addr[MAX_IP_ADDR];			// generic ip destination
	int ip_port;						// generic port destination
	char *cmd;							// request text
	// response
	ResponseEventHandler on_response;	// response callback
	int status;							// response status 
	char *resp;							// response text
	
	// connection support on libuv
	uv_connect_t req;
} msg_request_t;


typedef struct channel {
	uv_tcp_t socket;		 // the channel socket
    uv_write_t writereq;	 // Write request - must survive until write completion	
	char buffer[BUFFER_SIZE];// I/O buffer 
	int len;                 // buffer data length 
    bool valid;				 // valid state channel
    bool busy;				 // tells a request is active on channel
    session_t session;		 // associated session
	msg_request_t *msg;		 // the associated lib context
} channel_t;






typedef struct msg_peer_meeting_t {
	// request
	char ip_addr[MAX_IP_ADDR];
	int ip_port;
	// response
	int status;
	PeerSock other;
	PeerEventHandler on_meeting;
} msg_peer_meeting_t;


typedef struct msg_peer_recv {
	// request
	PeerSock other;
	// response
	ReadEventHandler on_response;
	int status;
	char *msg;
} msg_peer_recv_t;


typedef struct msg_peer_write{
	// request
	PeerSock other;
	char *msg;
	// response
} msg_peer_write_t;


#ifdef __cplusplus 
extern "C" {
#endif
void exec_request(msg_request_t *req);



void start_server();

#ifdef __cplusplus 
}
#endif
