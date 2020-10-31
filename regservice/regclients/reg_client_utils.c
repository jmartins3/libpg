 
 #ifndef _GNU_SOURCE
 #define _GNU_SOURCE
 #endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../include/reg_service.h"


void get_passwd(const char *user, char *pass) {
	sprintf(pass, "%s_pass", user);
}
	

//
// Sockets
// 

static int create_client_socket() {
	return socket(AF_INET, SOCK_STREAM, 0); 
}

static FILE *get_resp_stream(int cfd) {
	
	FILE *resp = fdopen(dup(cfd), "r");
	if (resp == NULL) {
		printf("exec_regist: error getting buffered response stream from socket|!\n");
		close(cfd);
		return NULL;
	}
	return resp;
}

typedef struct connection {
	int cfd; 		// connection socket
	FILE *resp; 	// response stream
} connection_t;


connection_t * connect_to(const char *server_ip_addr) {
	int cfd;
	
	if ((cfd=create_client_socket()) == -1) return NULL;
	
	struct sockaddr_in srv_addr;
	
	bzero(&srv_addr, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr(server_ip_addr);
	srv_addr.sin_port = htons(REG_SERVER_PORT);
		
	if (connect(cfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1) {
		close(cfd);
		return NULL;
	}
	
	FILE *resp = get_resp_stream(cfd);
	if (resp == NULL) {
		close(cfd);
		return NULL;
	}
	
	connection_t * cn = (connection_t *) malloc(sizeof(connection_t));
	cn->cfd = cfd;
	cn->resp = resp;
	
	return cn;
	 
}

void cn_close( connection_t *cn) {
	fclose(cn->resp);
	close(cn->cfd);
	free(cn);
}

//
// Commands

int show_response_header(FILE *resp) {
	// retrieve response
	char line[128];
	int status;
	// get response status
	fgets(line, 128, resp);
	 
	sscanf(line, "%d", &status);
	
	/*
	if (!SUCCESS(status)) printf("Error %d return by server\n", );
	else printf("cmd succceded with status %d\n", status);
	* */
	printf("%s", line);
	 
	return status;
}

int show_response_status(FILE *resp) {
	int res;
	char line[128];
	
	res = show_response_header(resp);
	// retrieve the terminator line
	fgets(line, 128, resp);
	return res;
}

int show_response_list(FILE *resp) {
	// retrieve response
	int res;
	char line[128];
	if ((res=show_response_header(resp)) != STATUS_OK) return res;
	printf("response:\n");
	while (fgets(line, 128, resp) && line[0] != '\n') {
		printf("%s\n", line);
	}
	return STATUS_OK;
}

int show_join_leave_response(FILE *resp) {
	// retrieve response
	int res, joiners;
	char line[128];
	if ((res=show_response_header(resp)) != STATUS_OK) return res;
	printf("response:\n");
	fgets(line, 128, resp);
	sscanf(line, "%d", &joiners);
	printf("there are %d joiners in topic\n", joiners);
	
	// retrieve the terminator line
	fgets(line, 128, resp);
	return STATUS_OK;
}


int exec_regist(connection_t *cn, const char *username, const char *pass, int number) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute registration!\n");
	int cmd_size = sprintf(cmd, "REGIST\n%d  %s  %s\n", number, username, pass);
	write(socket, cmd, cmd_size);
	
	//printf("3 seconds until message conclusion...");
	//sleep(3);
	
	write(socket, "\n", 1);
	
	return show_response_status(resp);
	
}


int exec_unregist(connection_t *cn, const char *username, const char *pass) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute unregistration!\n");
	int cmd_size = sprintf(cmd, "UNREGIST\n%s %s\n", username, pass);
	write(socket, cmd, cmd_size);
	
	//printf("3 seconds until message conclusion...");
	//sleep(3);
	
	write(socket, "\n", 1);
	
	return show_response_status(resp);
	
}

int exec_create_theme(connection_t *cn, 
					const char *username, const char *pass, const char *theme) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute create theme!\n");
	int cmd_size = sprintf(cmd, "CREATE_THEME\n%s %s\n%s\n\n", username, pass, theme);
	write(socket, cmd, cmd_size);
	
	return show_response_status(resp);
}


int exec_create_topic(connection_t *cn, 
					const char *username, const char *pass,
					const char *theme, const char *topic, int msg_port) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute create topic!\n");
	int cmd_size = sprintf(cmd, "CREATE_TOPIC\n%s %s\n%s %s\n%d\n\n", 
					username, pass, theme, topic, msg_port);
	write(socket, cmd, cmd_size);
	
	return show_response_status(resp);
}


int exec_join_topic(connection_t *cn, 
					const char *username, const char *pass,
					const char *theme, const char *topic, int msg_port) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute join topic!\n");
	int cmd_size = sprintf(cmd, "JOIN_TOPIC\n%s %s\n%s %s\n%d\n\n", 
					username, pass, theme, topic, msg_port);
	write(socket, cmd, cmd_size);
	
	return show_join_leave_response(resp);
}

int exec_list_themes(connection_t *cn,
				    const char *username, const char *pass) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute list themes!\n");
	int cmd_size = sprintf(cmd, "LIST_THEMES\n%s %s\n\n", username, pass);
	write(socket, cmd, cmd_size);
	
	return show_response_list(resp);
}

int exec_list_topics(connection_t *cn,
				     const char *username, const char *pass,
					 const char *theme ) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute list topics!\n");
	int cmd_size = sprintf(cmd, "LIST_TOPICS\n%s %s\n%s\n\n", username, pass, theme);
	write(socket, cmd, cmd_size);
	
	return show_response_list(resp);
}

int exec_remove_theme(connection_t *cn, 
					  const char *username, const char *pass,
					  const char *theme) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute remove theme!\n");
	int cmd_size = sprintf(cmd, "REMOVE_THEME\n%s %s\n%s\n\n", 
					username, pass, theme);
	write(socket, cmd, cmd_size);
	
	return show_response_status(resp);
}


int exec_remove_topic(connection_t *cn, 
					const char *username, const char *pass,
					const char *theme, const char *topic) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute remove topic!\n");
	int cmd_size = sprintf(cmd, "REMOVE_TOPIC\n%s %s\n%s %s\n\n", 
					username, pass, theme, topic);
	write(socket, cmd, cmd_size);
	
	return show_response_status(resp);
}

int exec_leave_topic(connection_t *cn, 
					const char *username, const char *pass,
					const char *theme, const char *topic) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute leave topic!\n");
	int cmd_size = sprintf(cmd, "LEAVE_TOPIC\n%s %s\n%s %s\n\n", 
					username, pass, theme, topic);
	write(socket, cmd, cmd_size);
	
	return show_join_leave_response(resp);
}


int exec_broadcast(connection_t *cn, 
					const char *username, const char *pass,
					const char *theme, const char *topic) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute broadcast on topic!\n");
	int cmd_size = sprintf(cmd, "BROADCAST\n"
	                            "%s %s\n%s %s\n"
	                            "line1 \n"
	                            "line2 \n"
	                            "\n", 
								username, pass, theme, topic);
	write(socket, cmd, cmd_size);
	
	return show_response_status(resp);
}

int exec_message(connection_t *cn, 
					const char *username, const char *pass,
					const char *theme, const char *topic, char *dest) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute unicat message on topic!\n");
	int cmd_size = sprintf(cmd, "MESSAGE\n"
	                            "%s %s\n%s %s %s\n"
	                            "line1 \n"
	                            "line2 \n"
	                            "\n", 
								username, pass, theme, topic, dest);
	write(socket, cmd, cmd_size);
	
	return show_response_status(resp);
}

int exec_stop(connection_t *cn ) {
	char cmd[128];
	int socket = cn->cfd;
	FILE *resp = cn->resp;
	
	printf("Execute stop server!\n");
	int cmd_size = sprintf(cmd, "STOP\n\n");
	write(socket, cmd, cmd_size);
	
 
	
	return show_response_status(resp);
	
}
