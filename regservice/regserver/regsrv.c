#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>


#include "../include/reg_service.h"
#include "channel.h"


extern bool verbose_mode;

#define DEBUG_C
#define DEBUG_R
#define DEBUG_CLOSE

#define DEFAULT_BACKLOG 128

static uv_loop_t loop;	// the libuv loop 
static uv_tcp_t server;	// connection libuv socket
 
static int active_count;
static bool terminated;

//
// Callbacks
//


static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	channel_t *chn = (channel_t *) handle;
    buf->base = chn->buffer;
    buf->len = BUFFER_SIZE;
}

void on_close_server(uv_handle_t* handle) {
	db_destroy();
	show_allocations();
}

void on_close(uv_handle_t* handle) {
#ifdef DEBUG_CLOSE
	if (verbose_mode)
		fprintf(stderr, "close handle %p\n", handle);
#endif
    chn_destroy(handle);
    active_count--;
    if (active_count == 0 && terminated)
		uv_close((uv_handle_t* ) &server, on_close_server);
}

void on_shutdown(uv_shutdown_t*req, int status) {
	uv_close((uv_handle_t*) req->handle, on_close);
}
	
void stop_server() {
	terminated = true;
}

static void read_completion(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
#ifdef DEBUG_R
	if (verbose_mode)
		printf("read completed!\n");
#endif
	channel_t *channel = (channel_t *) stream;
    if (nread > 0) {
		chn_process(channel, nread);
    }
    else if (nread <= UV_EOF) {
		uv_shutdown_t *req = (uv_shutdown_t *) malloc(sizeof(uv_shutdown_t));
		uv_shutdown(req, stream, on_shutdown);
	}
        
}
	
void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
#ifdef DEBUG_C
	if (verbose_mode)
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
#endif
        // error!
        return;
    }
	
    channel_t *chn = chn_create(&loop);
    if (!terminated && uv_accept(server, (uv_stream_t*) chn) == 0) {
#ifdef DEBUG_C
	if (verbose_mode)
		printf("start connection with new client!\n");
#endif
        uv_read_start((uv_stream_t*) chn, alloc_buffer, read_completion);
        active_count++;
    }
    else {
        uv_close((uv_handle_t*) chn, on_close);
    } 
}


static int server_init(const char *str_addr, int port) {
	struct sockaddr_in addr;

	uv_loop_init(& loop);
	uv_tcp_init(&loop, &server);
	uv_ip4_addr(str_addr, port, &addr);
	uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
	cmds_init(&loop);
	printf("start server listening on port %d...\n", port);
    return uv_listen((uv_stream_t*) &server, DEFAULT_BACKLOG, on_new_connection);
}


int main(int argc, char *argv[]) {
 	
	int r;
	
	if (argc == 2)  {
		printf("verbose_mode!\n");
		verbose_mode = true;
	}
 
	if ((r=server_init("0.0.0.0", REG_SERVER_PORT)) != 0) {
		fprintf(stderr, "Listen error %s\n", uv_strerror(r));
		return 1;
	}
   
    uv_run(&loop, UV_RUN_DEFAULT);
    
    uv_loop_close(&loop);
    printf("Server terminated!\n");
    return 0;
}
