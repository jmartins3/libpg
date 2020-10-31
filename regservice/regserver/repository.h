#pragma once

#include <uv.h>
#include "limits.h"
#include "../include/list.h"

/*
 * Model types: User, Theme, Topic
 */
 

// error codes
#define OPER_OK 0

// theme errors
#define THEME_INEXISTENT		41
#define THEME_DUPLICATE 		42
#define THEME_NOT_EMPTY			43
#define THEME_NAME_TOO_BIG		44


// topic errors
#define TOPIC_INEXISTENT		11
#define TOPIC_DUPLICATE 		12
#define TOPIC_NOT_EMPTY			13
#define TOPIC_NAME_TOO_BIG		14
#define TOPIC_DUPLICATE_JOINER	15
#define TOPIC_OWNER_NOT_LAST	16
#define TOPIC_USER_NOT_JOINER	17
#define TOPIC_NO_TOPICS			18
#define TOPIC_TOO_MANY_JOINERS	19
#define TOPIC_USER_ALONE		20


// user errors
#define USER_INEXISTENT			21
#define USER_DUPLICATE 			22
#define USER_NOT_THEME_OWNER 	23
#define USER_NOT_TOPIC_OWNER	24
#define USER_NAME_TOO_BIG		25
#define USER_PASS_TOO_BIG		26
#define USER_NOT_EMPTY			27
#define SEND_NOT_A_TOPIC_JOINER 28
#define DEST_NOT_A_TOPIC_JOINER 29

// general errors
#define INCONSISTENT_DB			31

typedef struct user {
	LIST_ENTRY link;				// the link node
	char name[MAX_USER_NAME];
	char passwd_hash[PASSWD_HASH_SIZE];
	int  number;
	int nthemes;					// total themes created
	int ntopics;					// total topics created
	int njoins;						// total joins on topics
} user_t;


 

typedef struct theme {
	LIST_ENTRY link;				// the link node
	char name[MAX_THEME_NAME+1];	// theme name
	LIST_ENTRY topics;				// topics list
	int ntopics;					// topics number
	user_t *owner_user;				// theme owner
} theme_t;


 
typedef struct topic {
	LIST_ENTRY link;				// the link node
	char name[MAX_TOPIC_NAME+1];	// topic name
	theme_t *belong_theme;
	user_t *owner_user;
	int njoiners;					// total topic joiners
	int limit;						// max topic joiners
	LIST_ENTRY joiners;				// topic joiners
} topic_t;


typedef struct topic_joiner {
	LIST_ENTRY link;				// the link node
	user_t *joiner;
	topic_t *topic;
	int msg_port;					// user udp port  for messaging
	struct sockaddr_in sock_addr;	// user ip address 
} topic_joiner_t;


typedef int (*cmp_node) (LIST_ENTRY *n1, void *val);	

typedef struct names_result {
	int nresults;	// number of names in collection
	int buf_size;
	char *buffer;	// the names collection
} names_result_t;

typedef struct joiner_info {
	char username[MAX_USER_NAME];	// user name
	int number;						// user number
	struct sockaddr_in sock_addr;	// user ip address 	
} joiner_info_t;


theme_t * theme_search(char *name);
	 

int theme_remove(char *name, user_t *remover);
	 

int theme_create(char *name, user_t *creator);
	 
int themes_collection(names_result_t *res);


//
// topics
//
 
	 
topic_t * topic_search(char *theme, char *topic);
 
	 
int topic_create(char *theme_name, char *name, struct sockaddr_in *sock_addr, user_t *creator);
 
	 
int topic_remove(char *theme_name, char *name, user_t* remover);


int topic_join(char *theme_name, char *name, struct sockaddr_in *sock_addr,  user_t *joining_user, int *njoiners);
	 

int topic_leave(char *theme_name, char *name, user_t *leaving_user, int *njoiners);
	 
int topics_collection(char *theme_name, names_result_t *res);

int topic_joiners(char *theme_name, char *topic_name, joiner_info_t **joiners, int *total, user_t *user);
 
int topic_owner_info(char *theme_name, char *name, joiner_info_t *owner_info);

int topic_joiner_search(char *theme_name, char *name, user_t *sender, char *user_dest, joiner_info_t *dest_info );
//
// users
//

user_t * user_get(char *name, char *passwd);
  
user_t * user_search(char *name);
	 

int user_create(char *name, char *passwd, int number );
	 

int user_remove(user_t *user);

int users_collection(names_result_t *res);


void db_init();
 
void db_destroy();
