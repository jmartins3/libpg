#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <uv.h>
#include "../include/socket_events.h"






static uv_loop_t loop;	// the libuv loop 
static pthread_t thread;
static uv_sem_t server_started;

// just for maintaining loop
static uv_tcp_t server;			// connection libuv socket



//
// Callbacks
//


static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	channel_t * chn = (channel_t*) handle;
	
	buf->base = chn->buffer + chn->len;
    buf->len = BUFFER_SIZE;
}

static void alloc_notification_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	session_t session = (session_t) handle->data;
	
	buf->base = session->notification_buffer;
    buf->len = NOTIFICATION_BUFFER_SIZE;
}


void on_close(uv_handle_t* handle) {
	fprintf(stderr, "close handle %p\n", handle);
	free(handle);
}

void on_notification(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
	session_t session = (session_t) handle->data;
	
	if (nread == 0) return;
	if (nread < 0) {
		session->notification = NULL;
	}
	else if (nread > 0) {
		session->notification_buffer[nread] = 0;
		session->notification = strdup(session->notification_buffer);
	}
	 
	send_graph_notification(session);	
}

 
void on_write(uv_write_t* req, int status) {
	channel_t * chn = (channel_t*) req->handle;
	msg_request_t *msg = (msg_request_t *) chn->msg;
	if (status) {
		fprintf(stderr, "uv_write error" );
		msg->status = status;
		send_graph_response(msg);
		return;
	}
	 
	printf("wrote on thread %ld.\n", pthread_self());
}


void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	
	printf("read completion on thread %ld.\n", pthread_self());	
	channel_t *chn = (channel_t*) client;
	msg_request_t *msg = (msg_request_t *) chn->msg;
	if (nread > 0) {
		chn->len += nread;
		// check if it is a communication chaneel with gorup server 
		// and the response is completed
		if (msg->session != NULL && chn->len > 2 && 
		    chn->buffer[chn->len-1] == '\n' &&  chn->buffer[chn->len-2] == '\n') {
			// send response
			msg->status = 0;
			chn->buffer[chn->len]=0;
			msg->resp = strdup(chn->buffer);
			send_graph_response(msg);
			chn->len = 0;	
		}
	}
	else if (nread < 0) {
		if (nread == UV_EOF) {
			msg->status = 0;
			msg->resp = strdup(chn->buffer);
		}
		else {
			msg->status = nread;
			msg->resp = strdup("Communication error");
		}
		chn->buffer[chn->len]=0;
		 
		send_graph_response(msg);
		
		// check if the handle is to maintain
		// (in case we have a session with the group server)
		if (msg->session  == NULL) {
			fprintf(stderr, "close handle %p\n", client);
			uv_close((uv_handle_t*)client, on_close);
		}
		
	}
	 
}

	
void on_connect(uv_connect_t* connection, int status)
{
	channel_t * chn = (channel_t*) connection->handle;
	msg_request_t *msg = (msg_request_t *) chn->msg;
	if (status != 0) { 
		printf("error %d on connection.\n", status); 
	
		msg->status = status;
		msg->resp = strdup("Error on connection");
		send_graph_response(msg);
		 
	}
	else {
		printf("connected!\n");
		uv_read_start((uv_stream_t *) chn, alloc_buffer, on_read);
		uv_buf_t buf;
		buf.base = msg->cmd;
		buf.len = strlen(msg->cmd);
		printf("send command!\n");
		uv_write(&chn->writereq, (uv_stream_t*) chn, &buf, 1, on_write);
		
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

static void connect_to(msg_request_t *msg ) {
	/// execute the request
	struct sockaddr_in dest;
	
	channel_t *chn = ( channel_t *) malloc(sizeof(channel_t));
	uv_tcp_init(&loop, (uv_tcp_t *) chn);
	chn->msg = msg;
	chn->len  = 0;
	
	// in case it is a channel to group server communication
	// we must create a udp socket for msg notifications
	
	if (msg->session != NULL) {
		msg->session->chn = chn;
		create_client_udp_socket(msg->session);
	}
	uv_ip4_addr(msg->ip_addr, msg->ip_port, &dest);
	
	uv_tcp_connect(&msg->req, (uv_tcp_t*) chn, (struct sockaddr *) &dest, on_connect);

}

// message request dispatcher at uv thread
void async_cb(uv_async_t * async)
{
	msg_request_t *msg = (msg_request_t*) async->data;
	switch(msg->type) {
		case GroupSrvConnect:
		case GenericRequest:
			connect_to(msg); break;
		case GroupSrvCmd:
			uv_buf_t buf;
			buf.base = msg->cmd;
			buf.len = strlen(msg->cmd);
			msg->session->chn->msg = msg;
			msg->session->chn->len  = 0;
			printf("send command: '%s'\n", msg->cmd);
			uv_write(&msg->session->chn->writereq, (uv_stream_t*) msg->session->chn, &buf, 1, on_write);
		default:
			break;
	}
  
}

void exec_request(msg_request_t *req) {
	uv_async_t * async = (uv_async_t*) malloc(sizeof(uv_async_t));
	
	async->data = req;
	
	printf("start exec_request  on thread %ld\n", pthread_self());
	
	uv_async_init(&loop, async, async_cb);
	
	uv_async_send(async);
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }
	
   
}

static void* server_init(void *arg) {
	struct sockaddr_in addr;
	 
	uv_loop_init(& loop);
	uv_tcp_init(&loop, &server);
	uv_ip4_addr("0.0.0.0", SERVER_PORT, &addr);
	uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
	
	
    uv_listen((uv_stream_t*) &server, DEFAULT_BACKLOG, on_new_connection);
    
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
	
