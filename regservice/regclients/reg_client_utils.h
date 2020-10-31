#pragma once


#include "../include/reg_service.h"


//
// Admin user
//
#define ADMIN "admin"
#define ADMIN_PASS "admin_pass"

//
// Sockets
// 

typedef struct connection {
	int cfd; 		// connection socket
	FILE *resp; 	// response stream
} connection_t;

connection_t * connect_to(const char *server_ip_addr);
void cn_close( connection_t *cn);

void get_passwd(const char *user, char *pass);

//
// Responses

int show_response_header(FILE *resp);

int show_response_status(FILE *resp);


int show_response_list(FILE *resp);

int show_join_leave_response(FILE *resp);


//
// Commands

int exec_stop(connection_t *cn );

int exec_regist(connection_t *cn, const char *username, const char *pass, int number);


int exec_unregist(connection_t *cn, const char *username, const char *pass);

int exec_create_theme(connection_t *cn, 
					const char *username, const char *pass, const char *theme);


int exec_create_topic(connection_t *cn, 
					const char *username, const char *pass,
					const char *theme, const char *topic, int msg_port);


int exec_join_topic(connection_t *cn, 
					const char *username, const char *pass,
					const char *theme, const char *topic, int msg_port);
					

int exec_list_themes(connection_t *cn,
				    const char *username, const char *pass);
				    

int exec_list_topics(connection_t *cn,
				     const char *username, const char *pass,
					 const char *theme );
					 

int exec_remove_theme(connection_t *cn, 
					  const char *username, const char *pass,
					  const char *theme);
	


int exec_remove_topic(connection_t *cn, 
					const char *username, const char *pass,
					const char *theme, const char *topic);
	 

int exec_leave_topic(connection_t *cn, 
					const char *username, const char *pass,
					const char *theme, const char *topic);
	 


 

