
#pragma once

#include <uv.h>
#include "repository.h"


#ifdef __cplusplus 

struct uv_tcp_handler {
public:	 
	uv_tcp_t chn;
	struct sockaddr_in addr;
	
	uv_tcp_handler(uv_tcp_t chn) {
		 this->chn = chn; 
		 int size = sizeof(struct sockaddr_in);
		 uv_tcp_getsockname(&chn, (struct sockaddr *) &addr, &size);
	}
};

extern std::map<uv_tcp_t*, user_session_t *> sessions;


extern "C" {
#endif

/**
 * obtem a sessão associada ao canal "chn"
 * Caso não exista ainda a sessão é criada uma nova sessão
 * para o utilizador "user"
 */
user_session_t* get_session(uv_tcp_t *chn, user_t *user);

user_session_t* find_session(uv_tcp_t *chn);

void session_destroy(user_session_t *session);


void session_add_topic(user_session_t *session, topic_t *topic);

void session_join_topic(user_session_t *session, topic_t *topic);

void session_remove_topic(user_session_t *session, topic_t *topic);

void session_leave_topic(user_session_t *session, topic_t *topic);

#ifdef __cplusplus 
}
#endif
