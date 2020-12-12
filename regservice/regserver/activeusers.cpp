#include <stdio.h>
#include <stdbool.h>
#include <map>

#include "../include/list.h"
#include <cstring>
#include "activeusers.h"

using namespace std;


#define DEBUG_SESSION


extern "C" {
bool verbose_mode = false;
}


typedef struct topic_entry {
	list_entry_t link;
	topic_t *topic;
} topic_entry_t;

 

typedef uv_tcp_t *uv_tcp_ptr_t;

namespace std {
	template<> struct  less<uv_tcp_ptr_t>
	{
		bool operator() (const uv_tcp_ptr_t lhs, uv_tcp_ptr_t rhs) const
		{
			return lhs  < rhs;
		}
	};
}

map<uv_tcp_ptr_t, user_session_t *> sessions;

 
static bool remove_topic(list_entry_t *list, topic_t *topic) {
	for(list_entry_t *curr = list->flink; curr != list; curr = curr->flink) {
		topic_entry_t *entry = (topic_entry_t*) curr;
		if (entry->topic == topic) {
			remove_entry_list(curr);
			return true;
		}
	}
	return false;
}


static user_session_t *create_session(uv_tcp_ptr_t chn, user_t *user) {
	user_session_t * new_session = (user_session_t*) malloc(sizeof(user_session_t));
	new_session->user = user;
	new_session->chn = chn;
	init_list_head(&new_session->transient_created_topics);
	init_list_head(&new_session->transient_joined_topics);
	return new_session;
}


void session_destroy(user_session_t *session) {
	// leave all joined topics

#ifdef DEBUG_SESSION
	if (verbose_mode)
		printf("leave joined topics for %s\n", session->user->name);
#endif
	for(list_entry_t *curr = session->transient_joined_topics.flink; 
				curr != &session->transient_joined_topics; curr = curr->flink) {
		topic_entry_t *entry = (topic_entry_t*) curr;
		topic_leave2(entry->topic, session->user);
	}
	
	// remove all created topics
	// leave all joined topics
#ifdef DEBUG_SESSION	
	if (verbose_mode)
		printf("destroy created topics for %s\n", session->user->name);
#endif
	for(list_entry_t *curr = session->transient_created_topics.flink; 
				curr != &session->transient_created_topics; curr = curr->flink) {
		topic_entry_t *entry = (topic_entry_t*) curr;
		topic_remove2(entry->topic, session->user);
	}
	
	// remove session from sessions
	
	sessions.erase(session->chn);
	free(session);
}

void session_add_topic(user_session_t *session, topic_t *topic) {
#ifdef DEBUG_SESSION
	if (verbose_mode)
		printf("created transient topic %s for user %s\n", topic->name, session->user->name);
#endif
	topic_entry_t *entry = (topic_entry_t *) malloc(sizeof(topic_entry_t));
	entry->topic = topic;
	insert_tail_list(&session->transient_created_topics, &entry->link);
}

void session_remove_topic(user_session_t *session, topic_t *topic) {
#ifdef DEBUG_SESSION
	if (verbose_mode)
		printf("remove transient topic %s for user %s\n", topic->name, session->user->name);
#endif
	remove_topic(&session->transient_created_topics, topic);
}

void session_leave_topic(user_session_t *session, topic_t *topic) {
#ifdef DEBUG_SESSION
	if (verbose_mode)
		printf("leave joined topic %s for user %s\n", topic->name, session->user->name);
#endif
	remove_topic(&session->transient_joined_topics, topic);
}


void session_join_topic(user_session_t *session, topic_t *topic) {
#ifdef DEBUG_SESSION
	if (verbose_mode)
		printf("topic %s joined for user %s\n", topic->name, session->user->name);
#endif
	topic_entry_t *entry = (topic_entry_t *) malloc(sizeof(topic_entry_t));
	entry->topic = topic;
	insert_tail_list(&session->transient_joined_topics, &entry->link);
}



user_session_t* find_session(uv_tcp_t *chn) {
	map<uv_tcp_ptr_t,user_session_t *>::iterator it;
	 
	//uv_tcp_handler handler(chn);
	if ((it = sessions.find(chn)) == sessions.end() )  
		return NULL;
	 
	return it->second;

}

user_session_t* get_session(uv_tcp_t *chn, user_t *user) {
	user_session_t *sess =  find_session(chn);
	
	if (sess == NULL) {
		//uv_tcp_handler handler(chn);
		sess = create_session(chn, user);
		sessions.insert(pair<uv_tcp_ptr_t,user_session_t *>(chn,sess));
	}
	 
	return sess;
}
