#include <stdio.h>
#include <map>

#include "../include/list.h"
#include <cstring>
#include "activeusers.h"

using namespace std;


typedef struct topic_entry {
	LIST_ENTRY link;
	topic_t *topic;
} topic_entry_t;

/*
namespace std {
	template<> struct  less<uv_tcp_handler>
	{
		bool operator() (const uv_tcp_handler& lhs, const uv_tcp_handler& rhs) const
		{
			return lhs.addr.sin_port <= rhs.addr.sin_port;
		}
	};
}
*/

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

 
static bool remove_topic(LIST_ENTRY *list, topic_t *topic) {
	for(LIST_ENTRY *curr = list->flink; curr != list; curr = curr->flink) {
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
	
	printf("leave joined topics for %s\n", session->user->name);
	for(LIST_ENTRY *curr = session->transient_joined_topics.flink; 
				curr != &session->transient_joined_topics; curr = curr->flink) {
		topic_entry_t *entry = (topic_entry_t*) curr;
		topic_leave2(entry->topic, session->user);
	}
	
	// remove all created topics
	// leave all joined topics
	printf("destroy created topics for %s\n", session->user->name);
	for(LIST_ENTRY *curr = session->transient_created_topics.flink; 
				curr != &session->transient_created_topics; curr = curr->flink) {
		topic_entry_t *entry = (topic_entry_t*) curr;
		topic_remove2(entry->topic, session->user);
	}
	
	// remove session from sessions
	
	sessions.erase(session->chn);
	free(session);
}

void session_add_topic(user_session_t *session, topic_t *topic) {
	printf("created transient topic %s for user %s\n", topic->name, session->user->name);
	topic_entry_t *entry = (topic_entry_t *) malloc(sizeof(topic_entry_t));
	entry->topic = topic;
	insert_tail_list(&session->transient_created_topics, &entry->link);
}

void session_remove_topic(user_session_t *session, topic_t *topic) {
	printf("remove transient topic %s for user %s\n", topic->name, session->user->name);
	remove_topic(&session->transient_created_topics, topic);
}

void session_leave_topic(user_session_t *session, topic_t *topic) {
	printf("leave joined topic %s for user %s\n", topic->name, session->user->name);
	remove_topic(&session->transient_joined_topics, topic);
}


void session_join_topic(user_session_t *session, topic_t *topic) {
	printf("topic %s joined for user %s\n", topic->name, session->user->name);
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
