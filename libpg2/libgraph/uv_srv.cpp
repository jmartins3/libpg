#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <uv.h>
#include "../include/socket_events.h"
#include <assert.h>

//#define DEBUG_OUTR

//#define DEBUG_OUTW

//#define DEBUG_OUTN


#ifndef STATUS_OK
#define STATUS_OK 201
#endif

static 	uv_async_t async_handle;

static uv_loop_t loop;	// the libuv loop 
static pthread_t thread;
static uv_sem_t server_started;

// just for maintaining loop
static uv_tcp_t server;			// connection libuv socket

// #define DEBUG_OUT2
// #define DEBUG_OUT1
// #define DEBUG_OUT 


static unsigned long old_number;
//
// Callbacks
//

/**
 * For simplicity, it is assumed that the buffer size is sufficient to 
 * accomodate any response, so we just advance the new buffer start
 * to the actual buffer length 
 */
static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	channel_t * chn = (channel_t*) handle;
	
	buf->base = chn->buffer + chn->len;
    buf->len = BUFFER_SIZE - chn->len;
}


static void alloc_notification_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	// TO DO
	// we assume the message is received completelly
	// The risk is not big sine the messages are very small
	// but in general this should not be done
	session_t session = (session_t) handle->data;
	
	buf->base = session->notification_buffer;
    buf->len = NOTIFICATION_BUFFER_SIZE;
}



void on_close(uv_handle_t* handle) {
 
	channel_t * chn = (channel_t*) handle;
	if (chn->session != NULL) {
		printf("on session close for %s\n", chn->session->user);
		chn->session->chn = NULL;
	    
		chn->msg->status = STATUS_OK;
		chn->msg->resp = NULL;
			++old_number;
		send_graph_response(chn->msg);
		 
	}
	free(handle);
}





void on_shutdown(uv_shutdown_t *req, int status) {
	
	uv_close((uv_handle_t*)req->handle, on_close);
	 
	free(req);
}


void on_notification(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
	session_t session = (session_t) handle->data;
	
	if (nread == 0) return;
	if (nread < 0) {
#ifdef DEBUG_OUTN	
	printf("error on notification msg!\n");	
#endif
		session->notification = NULL;
	}
	else if (nread > 0) {
		if (session->state != Closed) {
			session->notification_buffer[nread] = 0;
		
			save_notification(session);
	#ifdef DEBUG_OUTN	
		printf("notification msg: '%s'.\n", session->notification);		
	#endif
			send_graph_notification(session);
		}
	}
	 
	
}

 
void on_write(uv_write_t* req, int status) {
	channel_t * chn = (channel_t*) req->handle;
	chn->busy = false;
	free(req);
	msg_request_t *msg = (msg_request_t *) chn->msg;
	if (status) {
#ifdef DEBUG_OUTW
		fprintf(stderr, "uv_write error" );
#endif
		msg->status = status;
		send_graph_response(msg);
		return;
	}
	 
	
}

static void channel_set_invalid(channel_t *chn) {
	chn->valid = false;
	if (chn->session != NULL) chn->session->state = Closed;
}



void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {

#ifdef DEBUG_OUT1	
	printf("read completion on thread %ld.\n", pthread_self());	
#endif
	channel_t *chn = (channel_t*) client;
	msg_request_t *msg = (msg_request_t *) chn->msg;
	if (nread > 0) {
		chn->len += nread;
		// check if it is a communication chaneel with the group server 
		// and the response is completed
		if (msg->session != NULL && msg->session->state != Closed && chn->len > 2 && 
		    chn->buffer[chn->len-1] == '\n' &&  chn->buffer[chn->len-2] == '\n') {
			// send response
#ifdef DEBUG_OUTR	
	printf("read msg: '%s'.\n", chn->buffer);	
#endif
			if (old_number +1 != msg->number) {
					printf("old_number=%ld, msg_number=%ld\n", old_number, msg->number);
				
			}
			assert(++old_number == msg->number);
			msg->status = 0;
			chn->buffer[chn->len]=0;
			msg->resp = strdup(chn->buffer);
			 
			send_graph_response(msg);
			chn->len = 0;	
		}
	}
	else if (nread < 0) {
		if (msg != NULL) {
			if (nread == UV_EOF) {
				msg->status = 0;
				chn->buffer[chn->len]=0;
				msg->resp = strdup(chn->buffer);
			}
			else {
				msg->status = nread;
				msg->resp = strdup("Communication error");

			}
			
			send_graph_response(msg);
			
			
		}
		channel_set_invalid(chn);
		
		if (chn->session != NULL && !try_close_session(chn->session)) return;
		uv_shutdown_t *req = (uv_shutdown_t *) malloc(sizeof(uv_shutdown_t));
		uv_shutdown(req, (uv_stream_t*) client, on_shutdown);
		
		
	}
	 
}

	
void on_connect(uv_connect_t* connection, int status)
{
	channel_t * chn = (channel_t*) connection->handle;
	
	msg_request_t *msg = (msg_request_t *) chn->msg;
	if (status != 0) { 
#ifdef  DEBUG_OUT
		printf("error %d on connection.\n", status); 
#endif
		
		if (chn->session != NULL) {
			chn->session->chn = NULL;
			chn->session->state = Closed;
		}
		
		free(chn);
		msg->status = status;
		msg->resp = strdup("Error on connection");
		send_graph_response(msg);
		 
	}
	else {
		if (chn->session != NULL) chn->session->state = Connected;
//#ifdef DEBUG_OUT
		printf("connecttion done for %s!\n", msg->session->user);
//#endif
		uv_read_start((uv_stream_t *) chn, alloc_buffer, on_read);
		if (msg->cmd == NULL) {
			msg->status = 201;
			++old_number;
			msg->resp = strdup("Status ok");
			send_graph_response(msg);
		}
		else {
			uv_buf_t buf;
			buf.base = msg->cmd;
			buf.len = strlen(msg->cmd);
#ifdef DEBUG_OUTW
			printf("send command: '%s'\n", buf.base);
#endif
			uv_write_t *req = (uv_write_t *) malloc(sizeof(uv_write_t));
			//uv_write(&chn->writereq, (uv_stream_t*) chn, &buf, 1, on_write);
			uv_write(req, (uv_stream_t*) chn, &buf, 1, on_write);
		}
	}
	
}

void create_client_udp_socket(session_t session) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0); 
	if (sock < 0) return;
	struct sockaddr_in addr = {0};
	
	if (bind(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) != 0) {
		close(sock); return;
	}
 
	socklen_t len = sizeof(struct sockaddr_in);
	 
	if (getsockname( sock, (struct sockaddr *) &addr, &len) != 0) {
		close(sock); return;
	}
	uv_udp_init(&loop, &session->msg_sock);
	uv_udp_open(&session->msg_sock, sock);
	
    
	session->msg_sock.data = session;
	session->msg_port = addr.sin_port;
	uv_udp_recv_start(&session->msg_sock, alloc_notification_buffer, on_notification);
}

static channel_t *chn_create(msg_request_t *msg ) {
	channel_t *chn = ( channel_t *) malloc(sizeof(channel_t));
	uv_tcp_init(&loop, (uv_tcp_t *) chn);
	chn->msg = msg;
	chn->session = msg->session;
	chn->len  = 0;
	
	// in case it is a channel to group server communication
	// we must create a udp socket for msg notifications
	
	if (msg->session != NULL) {
		msg->session->chn = chn;
		create_client_udp_socket(msg->session);
	}
	chn->valid = true;
	return chn;
}

static void connect_to(msg_request_t *msg ) {
	/// execute the request
	struct sockaddr_in dest;
	
	channel_t *chn = chn_create(msg);
	 

	uv_ip4_addr(msg->ip_addr, msg->ip_port, &dest);
	
	uv_tcp_connect(&msg->req, (uv_tcp_t*) chn, (struct sockaddr *) &dest, on_connect);

}



// message request dispatcher at uv thread
void async_cb(uv_async_t * async)
{
	msg_request_t *msg = (msg_request_t*) async->data;
#ifdef DEBUG_OUT
	printf("do exec_request  on thread %ld\n", pthread_self());
	printf("old_number=%ld, msg_number=%ld\n", old_number, msg->number);
#endif

	
	uv_write_t *req;
	switch(msg->type) {
		case CloseChannel: {
			msg->session->chn->msg = msg;
		    printf("Close done!\n");
		  	uv_udp_recv_stop(&msg->session->msg_sock);
		  	uv_shutdown_t *req = (uv_shutdown_t *) malloc(sizeof(uv_shutdown_t));
			uv_shutdown(req, (uv_stream_t*) msg->session->chn, on_shutdown);
		
			break;
		}
		case GroupSrvConnect:
		case GenericRequest:
			connect_to(msg); 
			break;
		case GroupSrvCmd:
			uv_buf_t buf;
			buf.base = msg->cmd;
			buf.len = strlen(msg->cmd);
			msg->session->chn->msg = msg;
			msg->session->chn->len  = 0;
#ifdef DEBUG_OUTW
		printf("send command: '%s'!\n", buf.base);
#endif
			req = (uv_write_t*) malloc(sizeof(uv_write_t));
		 	
			uv_write(req, (uv_stream_t*) msg->session->chn, &buf, 1, on_write);
			break;
		default:
			break;
	}

  
}

void exec_request(msg_request_t *req) {
	if (req == NULL) return;
	
	async_handle.data = req;
#ifdef DEBUG_OUT
	printf("start exec_request  on thread %ld\n", pthread_self());
#endif
	
	
	
	uv_async_send(&async_handle);
}


static void* server_init(void *arg) {
	 
	uv_loop_init(& loop);
	uv_tcp_init(&loop, &server);
	uv_async_init(&loop, &async_handle, async_cb);
	
	
	
    uv_sem_post(&server_started);
	uv_run(&loop, UV_RUN_DEFAULT);
    
    uv_loop_close(&loop);
    return NULL;
}


void start_server() {
	 
	uv_sem_init(&server_started,0);
	pthread_create(&thread, NULL, server_init, NULL);
	
	uv_sem_wait(&server_started);
	uv_sem_destroy(&server_started);
}
	
