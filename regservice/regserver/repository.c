
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "repository.h"
#include "commands.h"
#include "activeusers.h"

//
// globals

static int nthemes;
static int nusers;
static list_entry_t themes;	// themes collection
static list_entry_t users;	// users collection


#define DEBUG_MEM

static void topic_internal_destroy(topic_t *topic);

extern bool verbose_mode;


//
// auxiliary functions
//




/**
 * retrieve the joiner user name
 * for transient joins this must be getted from session
 */
char *topic_joiner_name(topic_joiner_t *tje) {
	if (tje->topic->is_persistent) return tje->joiner->name;
	return tje->session->user->name;
}

/**
 * retrieve the joiner user 
 * for transient joins this must be getted from session
 */
user_t *topic_joiner_entity(topic_joiner_t *tje) {
	if (tje->topic->is_persistent) return tje->joiner;
	return tje->session->user;
}



/// generic search
static list_entry_t *search_generic(list_entry_t *list, void *val, cmp_node cmp) {
	for(list_entry_t *curr = list->flink; curr != list; curr = curr->flink) {
		if (cmp(curr, val) == 0) return curr;
	}
	return NULL;
}

/**
 * called when a topic has  joiners in addition to the owner
 */ 
static void topic_fill_joiners(topic_t *topic, user_t *user, joiner_info_t **joiners, int *total) {
#ifdef DEBUG_MEM
	if (verbose_mode) 
		printf("malloc: joiners for topic_joiners\n"); 
	inc_mallocs();
#endif
	*joiners = (joiner_info_t*) malloc(sizeof(joiner_info_t)*topic->njoiners-1);
	int pos = 0;
	for(list_entry_t *curr = topic->joiners.flink; curr != &topic->joiners; curr = curr->flink) {
		topic_joiner_t *tj= container_of(curr, topic_joiner_t, link);
		if (user != topic_joiner_entity(tj)) { // not broadcast to the emitter
			strcpy((*joiners)[pos].username, topic_joiner_name(tj));
			(*joiners)[pos].number = tj->joiner->number;
			(*joiners)[pos].sock_addr= tj->sock_addr;
			pos++;
		}
	}
	*total = pos;
}


//
// themes
//


static int cmp_by_theme_name(list_entry_t *curr, void *val) {
	char *name = (char*) val;
	theme_t *theme = container_of(curr, theme_t, link);
	
	return strcmp(theme->name, name);
}

theme_t * theme_search(char *name) {
	list_entry_t *node = search_generic(&themes, name, cmp_by_theme_name);
	if (node == NULL) return NULL;
	return container_of(node, theme_t, link);
}

int theme_remove(char *name, user_t *remover) {   
	theme_t *theme = theme_search(name);
	if ( theme == NULL) return THEME_INEXISTENT;
	if ( theme->owner_user != remover) return USER_NOT_THEME_OWNER;
	if ( theme->ntopics > 0) return THEME_NOT_EMPTY;
	
	remover->nthemes--;
	nthemes--;
	remove_entry_list(&theme->link);
#ifdef DEBUG_MEM
	if (verbose_mode) 
		printf("free: theme_remove\n"); 
	inc_frees();
#endif
	free(theme);
	return OPER_OK;
}

int theme_create(char *name, user_t *creator) {
	
	if (strlen(name) > MAX_THEME_NAME) return THEME_NAME_TOO_BIG;
	if (theme_search(name) != NULL) return THEME_DUPLICATE;
#ifdef DEBUG_MEM	
	if (verbose_mode) 
		printf("malloc: theme for theme_create\n"); 
	inc_mallocs();	 
#endif
	theme_t *theme = (theme_t *) malloc(sizeof(theme_t));
	strcpy(theme->name,name);
	theme->ntopics = 0;
	theme->owner_user = creator;
	init_list_head(&theme->topics);
	insert_tail_list(&themes, &theme->link);
	creator->nthemes++;
	nthemes++;
	return OPER_OK;
}


int themes_collection(names_result_t *res) {
	// create buffer  for max size;
#ifdef DEBUG_MEM	
	if (verbose_mode)
		printf("malloc: buffer for themes_collection\n"); 
	inc_mallocs();
#endif
	char *buf = 
		(char*) malloc((MAX_THEME_NAME+ MAX_USER_NAME+10)*nthemes);
	int pos = 0;
	for(list_entry_t *curr = themes.flink; curr != &themes; curr = curr->flink) {
		theme_t *theme = container_of(curr, theme_t, link);

		pos += sprintf(buf + pos, "%s %s %d\n", 
						theme->name, theme->owner_user->name, theme->ntopics);  
		 
	}
	res->nresults = nthemes;
	res->buf_size = pos;
	res->buffer = buf;
	return OPER_OK;
}

//
// topic joiners
//

static int cmp_by_user(list_entry_t *curr, void *val) {
	user_t *user = (user_t *) val;
	topic_joiner_t *tu = container_of(curr, topic_joiner_t, link);
	
	return strcmp(topic_joiner_name(tu), user->name);
}
  
static topic_joiner_t * topic_joiner_internal_search(list_entry_t *joiners, user_t *user) {
	list_entry_t *node =  search_generic(joiners, user, cmp_by_user);
	if (node == NULL) return NULL;
	return container_of(node,topic_joiner_t, link);
}



//
// topics
//

static int cmp_by_topic_name(list_entry_t *curr, void *val) {
	char *name = (char*) val;
	topic_t *topic = container_of(curr, topic_t, link);
	
	return strcmp(topic->name, name);
}

static topic_t * topic_internal_search(list_entry_t *topics, char *name) {
	list_entry_t *node = search_generic(topics, name, cmp_by_topic_name);
	if (node == NULL) return NULL;
	return container_of(node, topic_t, link);
}

static void topic_join_internal(topic_t * topic, struct sockaddr_in *sock_addr, void *content) {
#ifdef DEBUG_MEM		
	if (verbose_mode) 
		printf("malloc: topic_joiner_t for topic_join_internal\n"); 
	inc_mallocs();
#endif
	topic_joiner_t * tj = (topic_joiner_t * ) malloc(sizeof(topic_joiner_t));
	
	tj->content = content;
	tj->topic = topic;
	tj->sock_addr = *sock_addr;
	
 	
	insert_tail_list(&topic->joiners, &tj->link);
	
	if (topic->is_persistent) ((user_t*) content)->njoins++;
	else  ((user_session_t*) content)->user->njoins++;
	topic->njoiners++;

}

int topic_leave_internal(topic_t *topic, user_t *leaving_user) {
	topic_joiner_t * tj = topic_joiner_internal_search(&topic->joiners, leaving_user);
	if (tj == NULL) return TOPIC_USER_NOT_JOINER;
	if (!topic->is_persistent) session_leave_topic(tj->session, topic);
	remove_entry_list(&tj->link);
#ifdef DEBUG_MEM	
	if (verbose_mode) 
		printf("free: topic_leave\n"); 
	inc_frees();
#endif
	free(tj);
	leaving_user->njoins--;
	topic->njoiners--;
	return OPER_OK;
}
	 
topic_t * topic_search(char *theme_name, char *topic_name) {
	theme_t *theme = theme_search(theme_name);
	if (theme == NULL) return NULL;
	return topic_internal_search(&theme->topics, topic_name);
}
	 
int topic_create(char *theme_name, char *name, struct sockaddr_in *sock_addr, 
			user_session_t *session, char *owner) {
	
	user_t* creator = session->user;
	if (strlen(name) > MAX_TOPIC_NAME) return TOPIC_NAME_TOO_BIG;
	
	theme_t *theme = theme_search(theme_name);
	if (theme == NULL) return THEME_INEXISTENT;
	
	topic_t *old_topic;
	if ((old_topic = topic_internal_search(&theme->topics, name)) != NULL) {
		strcpy(owner, old_topic->owner_user->name);
		return TOPIC_DUPLICATE;
	}
#ifdef DEBUG_MEM	
	if (verbose_mode) 
		printf("malloc: topic_t for topic_create\n"); 
	inc_mallocs();
#endif
	topic_t *topic = (topic_t *) malloc(sizeof(topic_t));
	strcpy(topic->name, name);
	topic->belong_theme = theme;
	topic->njoiners = 0;
	topic->owner_user = creator;
	topic->is_persistent = false;
	init_list_head(&topic->joiners);
	insert_tail_list(&theme->topics, &topic->link);
	theme->ntopics++;
	
	
	creator->ntopics++;
	
	// the topic creator is implicitly the first topic joiner
	topic_join_internal(topic, sock_addr, topic->is_persistent ? (void*) creator : (void*)session);
	
	// add this topic to the owner session transient topics
	session_add_topic(session, topic);
	return OPER_OK;
}

void topic_remove2(topic_t *topic, user_t* remover) {
	
	
	// first, try to send a notification to all topic partners
	if (topic->njoiners > 1) {
		joiner_info_t *joiners;
		int total;
		topic_fill_joiners(topic, remover, &joiners, &total);
		warn_topic_partners(topic, joiners, total);
	}
	
	topic_internal_destroy(topic);
	 
	
	remove_entry_list(&topic->link);
	topic->belong_theme->ntopics--;
	remover->ntopics--;
#ifdef DEBUG_MEM		
	if (verbose_mode) 
		printf("free: topic_destroy\n"); 
	inc_frees();
#endif
	free(topic);
	

}

int topic_remove(char *theme_name, char *name, user_session_t * session) {
	user_t* remover = session->user;
	theme_t *theme = theme_search(theme_name);
	if (theme == NULL) return THEME_INEXISTENT;
	
	topic_t * topic =  topic_internal_search(&theme->topics, name);	
	if (topic == NULL) return TOPIC_INEXISTENT;
	
	if (topic->owner_user != remover) return USER_NOT_TOPIC_OWNER;
	if (topic->njoiners > 1) return TOPIC_NOT_EMPTY;


	if (topic_leave_internal(topic, remover) != OPER_OK)
		return INCONSISTENT_DB;
	
	session_remove_topic(session, topic);
	remove_entry_list(&topic->link);
	
#ifdef DEBUG_MEM		
	if (verbose_mode) 
		printf("free: topic_remove\n"); 
	inc_frees();
#endif
	free(topic);
	remover->ntopics--;
	theme->ntopics--;
	return OPER_OK;
}


int topic_destroy(char *theme_name, char *name, user_session_t* session) {
	user_t* remover = session->user;
	theme_t *theme = theme_search(theme_name);
	if (theme == NULL) return THEME_INEXISTENT;
	
	topic_t * topic =  topic_internal_search(&theme->topics, name);	
	if (topic == NULL) return TOPIC_INEXISTENT;
	
	if (topic->owner_user != remover) return USER_NOT_TOPIC_OWNER;
	
	
	session_remove_topic(session, topic);
	 
	topic_internal_destroy(topic);
	 
	
	remove_entry_list(&topic->link);
#ifdef DEBUG_MEM		
	if (verbose_mode) 
		printf("free: topic_destroy\n"); 
	inc_frees();
#endif
	free(topic);
	remover->ntopics--;
	theme->ntopics--;
	
	
	return OPER_OK;
}

void internal_owner_info(topic_t * topic, joiner_info_t *owner_info) {
	// the owner is by definition at the  top of the list
    topic_joiner_t *owner_entry = container_of(topic->joiners.flink, topic_joiner_t, link);
    
	strcpy(owner_info->username, topic_joiner_name(owner_entry));
			 
	owner_info->sock_addr= owner_entry->sock_addr;
}

int topic_owner_info(char *theme_name, char *name, joiner_info_t *owner_info) {
	theme_t *theme = theme_search(theme_name);
	if (theme == NULL) return THEME_INEXISTENT;
	
	topic_t * topic =  topic_internal_search(&theme->topics, name);	
	if (topic == NULL) return TOPIC_INEXISTENT;
	
	internal_owner_info(topic, owner_info);
	return OPER_OK;
}


int topic_join(char *theme_name, char *name, struct sockaddr_in *sock_addr, 
		user_session_t *session, int *njoiners) {
	
	user_t* joining_user = session->user;
	theme_t *theme = theme_search(theme_name);
	if (theme == NULL) return THEME_INEXISTENT;
	
	topic_t * topic =  topic_internal_search(&theme->topics, name);	
	if (topic == NULL) return TOPIC_INEXISTENT;
	
	if (topic->limit != 0 && topic->njoiners == topic->limit) 
		return TOPIC_TOO_MANY_JOINERS;
	if (topic_joiner_internal_search(&topic->joiners, joining_user) != NULL) 
		return TOPIC_DUPLICATE_JOINER;
	
	topic_join_internal(topic, sock_addr, topic->is_persistent ? (void*)joining_user: (void*)session);
	
	session_join_topic(session, topic);
	
	*njoiners = topic->njoiners;
	return OPER_OK;
}


int topic_joiner_search(char *theme_name, char *name, user_t *sender, char *user_dest, joiner_info_t *dest_info ) {
	theme_t *theme = theme_search(theme_name);
	if (theme == NULL) return THEME_INEXISTENT;
	
	topic_t * topic =  topic_internal_search(&theme->topics, name);	
	if (topic == NULL) return TOPIC_INEXISTENT;
	
	topic_joiner_t *sender_joiner = topic_joiner_internal_search(&topic->joiners, sender);
	if (sender_joiner == NULL) return SEND_NOT_A_TOPIC_JOINER;
	user_t * dest = user_search(user_dest);
	 
	if (dest == NULL) return USER_INEXISTENT; 
	topic_joiner_t *dest_joiner = topic_joiner_internal_search(&topic->joiners, dest);
	if (dest_joiner == NULL) return DEST_NOT_A_TOPIC_JOINER;
	strcpy(dest_info->username, user_dest);
			 
	dest_info->sock_addr= dest_joiner->sock_addr;
	return OPER_OK;
}
 
void topic_leave2(topic_t *topic, user_t *leaving_user) {
	joiner_info_t owner_info;
	
	internal_owner_info(topic, &owner_info);
	warn_topic_owner(topic, &owner_info, topic->njoiners);
	topic_leave_internal(topic, leaving_user);
}

int topic_leave(char *theme_name, char *name, user_session_t *session, int *njoiners) {
	user_t* leaving_user = session->user;
	theme_t *theme = theme_search(theme_name);
	if (theme == NULL) return THEME_INEXISTENT;
	
	topic_t * topic =  topic_internal_search(&theme->topics, name);	
	if (topic == NULL) return TOPIC_INEXISTENT;
	
	if (topic->owner_user == leaving_user && topic->njoiners > 1) 
		return TOPIC_OWNER_NOT_LAST;
	
	
	int res;
	if ((res = topic_leave_internal(topic, leaving_user)) != OPER_OK)
		return res;
	*njoiners = topic->njoiners;
	return OPER_OK;
 
}



int topics_collection(char *theme_name, names_result_t *res) {
	theme_t *theme = theme_search(theme_name);
	if (theme == NULL) return THEME_INEXISTENT;
	int pos = 0;
	char *buf = NULL;
	// create buffer  for max size;
	if (theme->ntopics > 0)  {
#ifdef DEBUG_MEM	
		if (verbose_mode) 
			printf("malloc: buffer for topics_collection\n"); 
		inc_mallocs();
#endif
		buf = (char*) malloc((MAX_TOPIC_NAME+1+ (MAX_USER_NAME*10)+ 5)*theme->ntopics);
	
		for(list_entry_t *curr = theme->topics.flink; curr != &theme->topics; curr = curr->flink) {
			topic_t *topic = container_of(curr, topic_t, link);
			
			pos += sprintf(buf + pos, "%s %s", topic->name, topic->owner_user->name);  
			
			for(list_entry_t *curr = topic->joiners.flink; curr != &topic->joiners; curr = curr->flink) {
				topic_joiner_t *tj= container_of(curr, topic_joiner_t, link);
				if (topic->owner_user != topic_joiner_entity(tj)) { // not broadcast to the emitter
					pos += sprintf(buf + pos, " %s", topic_joiner_name(tj));
				}
			}
			pos += sprintf(buf + pos, " %d\n", topic->njoiners);  
		}
		
	}
	res->nresults = theme->ntopics;
	res->buf_size = pos;
	res->buffer = buf;
	return OPER_OK;
}



int topic_joiners(char *theme_name, char *topic_name, joiner_info_t **joiners, int *total, user_t *user) {
	theme_t *theme = theme_search(theme_name);
	if (theme == NULL) return THEME_INEXISTENT;
	
	topic_t * topic =  topic_internal_search(&theme->topics, topic_name);	
	if (topic == NULL) return TOPIC_INEXISTENT;
	
	topic_joiner_t * tj_sender = topic_joiner_internal_search(&topic->joiners, user);
	if (tj_sender == NULL) return TOPIC_USER_NOT_JOINER;
	
	if (topic->njoiners == 1) return TOPIC_USER_ALONE;
	
	// create the result array
	topic_fill_joiners(topic, user, joiners, total);
	
	
	return OPER_OK;
}

//
// users
//

static int cmp_by_user_name(list_entry_t *curr, void *val) {
	char *name = (char*) val;
	user_t *user = container_of(curr, user_t, link);
	
	return strcmp(user->name, name);
}
  
user_t * user_search(char *name) {
	list_entry_t *node =  search_generic(&users, name, cmp_by_user_name);
	if (node == NULL) return NULL;
	return container_of(node, user_t, link);
}

user_t * user_get(char *name, char *passwd) {
	user_t *user = user_search(name);
	if (user == NULL) return NULL;
	if (strcmp(passwd, user->passwd_hash) != 0) return NULL;
	return user;
}

int user_create(char *name, char *passwd, int number ) {
	if (strlen(name) > MAX_USER_NAME ) return USER_NAME_TOO_BIG;
	if (strlen(passwd) > MAX_PASSWORD_SIZE) return USER_PASS_TOO_BIG;
	if (user_search(name) != NULL) return USER_DUPLICATE; // name already exists
#ifdef DEBUG_MEM	
	if (verbose_mode) 
		printf("malloc: user for user_create\n"); 
	inc_mallocs();
#endif
	user_t *user = (user_t *) malloc(sizeof(user_t));
	strcpy(user->name,name);
	// TODO- calculate the pasword hash
	strcpy(user->passwd_hash,passwd);
	user->number = number;
	user->nthemes = 0;
	user->ntopics = 0;
	user->njoins  = 0;
 
	insert_tail_list(&users, &user->link);
	nusers++;
	return OPER_OK;
}

int user_remove(user_t* user) {
	if (user_search(user->name) == NULL) return USER_INEXISTENT;
	 
	if (user->njoins > 0  ||
	    user->nthemes > 0 || 
	    user->ntopics > 0) return USER_NOT_EMPTY;
	 
	remove_entry_list(&user->link);
#ifdef DEBUG_MEM	
	if (verbose_mode) 
		printf("free: user\n"); 
	inc_frees();
#endif
	free(user);
	nusers--;
	return OPER_OK;
	 
}

int users_collection(names_result_t *res) {
	// create buffer  for max size;
	char *buf = NULL;
	int pos = 0;
	
	if (nusers > 0) {
#ifdef DEBUG_MEM	
		if (verbose_mode) 
			printf("malloc: buffer for users_collection\n"); 
		inc_mallocs();
#endif
		buf = (char*) malloc( (MAX_USER_NAME+10)*nusers);
		for(list_entry_t *curr = users.flink; curr != &users; curr = curr->flink) {
			user_t *user = container_of(curr, user_t, link);

			pos += sprintf(buf + pos, "%s %d\n", 
							user->name, user->number);  
			 
		}
	}
	res->nresults = nusers;
	res->buf_size = pos;
	res->buffer = buf;
	return OPER_OK;
}



void db_init() {
	init_list_head(&themes);
	init_list_head(&users);
	nthemes = 0;
	nusers = 0;
}

static void destroy_list(list_entry_t *list) {
	list_entry_t *curr = list->flink;
	while(curr != list) {
		list_entry_t *rem = curr;
		curr = curr->flink;
#ifdef DEBUG_MEM	
		if (verbose_mode) 
			printf("free: destroy_list_elem\n"); 
		inc_frees();
#endif
		free(rem);
	}
}

static void topic_internal_destroy(topic_t *topic) {
	for(list_entry_t *curr = topic->joiners.flink; 
				curr != &topic->joiners; curr = curr->flink) {
		topic_joiner_t *entry = (topic_joiner_t*) curr;
		if (!topic->is_persistent)
			session_leave_topic(entry->session, topic);
		
	}
}


static void topics_destroy(list_entry_t *list) {
	list_entry_t *curr = list->flink;
	while(curr != list) {
		list_entry_t *rem = curr;
		topic_t *topic = container_of(curr, topic_t, link);
		
		topic_internal_destroy(topic);
		curr = curr->flink;
#ifdef DEBUG_MEM	
		if (verbose_mode) 
			printf("free: topic\n"); 
		inc_frees();
#endif
		free(rem);
	}
}

static void themes_destroy() {
	list_entry_t *curr = themes.flink;
	while(curr != &themes) {
		list_entry_t *rem = curr;
		theme_t *theme = container_of(curr, theme_t, link);
		
		topics_destroy(&theme->topics);
		curr = curr->flink;
#ifdef DEBUG_MEM	
		if (verbose_mode) 
			printf("free: theme\n"); 
		inc_frees();
#endif
		free(rem);
	}
}

void db_destroy() {
	themes_destroy();
	destroy_list(&users);
}
