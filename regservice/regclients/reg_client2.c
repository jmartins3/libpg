/*
 * implements an echo client using a stream IP socket 
 */
 
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

#define REG_SERVER_ADDR "192.168.1.134"
#define MAX_SOCK_NAME 256
#define CLIENT_SOCK_PREFIX "sock_client_"

#define NITERS 10000


pthread_t thread;

int create_client_socket() {
	return socket(AF_INET, SOCK_STREAM, 0); 
}

int create_client_udp_socket(int *port) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0); 
	if (sock < 0) return sock;
	struct sockaddr_in addr = {0};
	socklen_t len = sizeof(struct sockaddr_in);
	if (bind(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) != 0) {
		close(sock); return -1;
	}
	if (getsockname( sock, (struct sockaddr *) &addr, &len) != 0) {
		close(sock); return -1;
	}
	*port = addr.sin_port;
	return sock;
	
}

#define SUCCESS(s) ( ((s) / 100) == 2)

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


int exec_regist(int socket, FILE *resp, const char *username, const char *pass, int number) {
	char cmd[128];
	
	printf("Execute registration!\n");
	int cmd_size = sprintf(cmd, "REGIST\n%d  %s  %s\n", number, username, pass);
	write(socket, cmd, cmd_size);
	
	//printf("3 seconds until message conclusion...");
	//sleep(3);
	
	write(socket, "\n", 1);
	
	return show_response_status(resp);
	
}


int exec_unregist(int socket, FILE *resp, const char *username, const char *pass) {
	char cmd[128];
	
	printf("Execute unregistration!\n");
	int cmd_size = sprintf(cmd, "UNREGIST\n%s %s\n", username, pass);
	write(socket, cmd, cmd_size);
	
	//printf("3 seconds until message conclusion...");
	//sleep(3);
	
	write(socket, "\n", 1);
	
	return show_response_status(resp);
	
}

int exec_create_theme(int socket, FILE *resp, 
					const char *username, const char *pass, const char *theme) {
	char cmd[128];
	
	printf("Execute create theme!\n");
	int cmd_size = sprintf(cmd, "CREATE_THEME\n%s %s\n%s\n\n", username, pass, theme);
	write(socket, cmd, cmd_size);
	
	return show_response_status(resp);
}


int exec_create_topic(int socket, FILE *resp, 
					const char *username, const char *pass,
					const char *theme, const char *topic, int msg_port) {
	char cmd[128];
	
	printf("Execute create topic!\n");
	int cmd_size = sprintf(cmd, "CREATE_TOPIC\n%s %s\n%s %s\n%d\n\n", 
					username, pass, theme, topic, msg_port);
	write(socket, cmd, cmd_size);
	
	return show_response_status(resp);
}


int exec_join_topic(int socket, FILE *resp, 
					const char *username, const char *pass,
					const char *theme, const char *topic, int msg_port) {
	char cmd[128];
	
	printf("Execute join topic!\n");
	int cmd_size = sprintf(cmd, "JOIN_TOPIC\n%s %s\n%s %s\n%d\n\n", 
					username, pass, theme, topic, msg_port);
	write(socket, cmd, cmd_size);
	
	return show_join_leave_response(resp);
}

int exec_list_themes(int socket, FILE *resp,
				    const char *username, const char *pass) {
	char cmd[128];
	
	printf("Execute list themes!\n");
	int cmd_size = sprintf(cmd, "LIST_THEMES\n%s %s\n\n", username, pass);
	write(socket, cmd, cmd_size);
	
	return show_response_list(resp);
}

int exec_list_topics(int socket, FILE *resp,
				     const char *username, const char *pass,
					 const char *theme ) {
	char cmd[128];
	
	printf("Execute list topics!\n");
	int cmd_size = sprintf(cmd, "LIST_TOPICS\n%s %s\n%s\n\n", username, pass, theme);
	write(socket, cmd, cmd_size);
	
	return show_response_list(resp);
}

int exec_remove_theme(int socket, FILE *resp, 
					  const char *username, const char *pass,
					  const char *theme) {
	char cmd[128];
	
	printf("Execute remove theme!\n");
	int cmd_size = sprintf(cmd, "REMOVE_THEME\n%s %s\n%s\n\n", 
					username, pass, theme);
	write(socket, cmd, cmd_size);
	
	return show_response_status(resp);
}


int exec_remove_topic(int socket, FILE *resp, 
					const char *username, const char *pass,
					const char *theme, const char *topic) {
	char cmd[128];
	
	printf("Execute remove topic!\n");
	int cmd_size = sprintf(cmd, "REMOVE_TOPIC\n%s %s\n%s %s\n\n", 
					username, pass, theme, topic);
	write(socket, cmd, cmd_size);
	
	return show_response_status(resp);
}

int exec_leave_topic(int socket, FILE *resp, 
					const char *username, const char *pass,
					const char *theme, const char *topic) {
	char cmd[128];
	
	printf("Execute leave topic!\n");
	int cmd_size = sprintf(cmd, "LEAVE_TOPIC\n%s %s\n%s %s\n\n", 
					username, pass, theme, topic);
	write(socket, cmd, cmd_size);
	
	return show_join_leave_response(resp);
}


int exec_broadcast(int socket, FILE *resp, 
					const char *username, const char *pass,
					const char *theme, const char *topic) {
	char cmd[128];
	
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

int exec_message(int socket, FILE *resp, 
					const char *username, const char *pass,
					const char *theme, const char *topic, char *dest) {
	char cmd[128];
	
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


typedef struct listener_ctx {
	int sock;
} listener_ctx_t;

static void* msg_listener(void *arg) {
	listener_ctx_t*ctx = (listener_ctx_t*) arg;
	int sock = ctx->sock;
	char buffer[256] = {0};
	
	while(true) {
		int nread;
		
		if ((nread = recv(sock, buffer, 256, 0)) < 0) 
			printf("error reading datagram socket\n");
		else 
			printf("read %d bytes from datagram socket: %s\n", nread, buffer);
	}
			
	return NULL;
}

void start_udp_listener(int sock) {
	listener_ctx_t *ctx = (listener_ctx_t*) malloc(sizeof(listener_ctx_t));
	ctx->sock = sock;
	
	pthread_create(&thread, NULL, msg_listener, ctx);
}
	
int main(int argc, char *argv[]) {
	int number;
	char *user, *pass;
	if (argc != 4) {
		printf("usage: reg_client2 <number> <user> <pass> \n");
		return 1; 
	}
	
	number = atoi(argv[1]);
	user = argv[2];
	pass = argv[3];
	
	struct sockaddr_in srv_addr;
	int cfd;
	 
	if ((cfd = create_client_socket()) < 0) {
		fprintf(stderr, "error creating socket\n");
		return 1;
	}
	int msg_port;
	
	int msg_sock = create_client_udp_socket(&msg_port);
	if (msg_sock < 0) {
		fprintf(stderr, "error creating messaging udp socket\n");
		return 1;
	}
	
	start_udp_listener(msg_sock);
	char *server_ip_addr = REG_SERVER_ADDR;
	
	/* Construct server address, and make the connection */
	bzero(&srv_addr, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr(server_ip_addr);
	srv_addr.sin_port = htons(REG_SERVER_PORT);
		
	if (connect(cfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1) {
		perror("Error connecting socket\n");
		return 2;
	}
	
	FILE *resp = fdopen(dup(cfd), "r");
	if (resp == NULL) {
		printf("exec_regist: error getting buffered response stream from socket|!\n");
		close(cfd);
		return 0;
	}
		 
	if (exec_regist(cfd, resp, user, pass, number) != STATUS_OK) return 1;
 	
 	bool creator;	 
 	
 	exec_create_theme(cfd, resp,  user, pass, "battleship");
 	
	if (exec_create_topic(cfd, resp,  user, pass, "battleship", "battle1", msg_port) == STATUS_OK) {
		printf("topic creator!\n");
		creator = true;
	}
	else {
		if (exec_join_topic(cfd, resp, user, pass, "battleship", "battle1", msg_port) != STATUS_OK) return 1;
		creator = false;
	}
	
	if (exec_list_themes(cfd, resp, user, pass)!= STATUS_OK) return 1;
	
	if (exec_list_topics(cfd, resp, user, pass, "battleship" )!= STATUS_OK) return 1;
	
	if (creator) {
		printf("press enter to broadcast...");
		getchar();
		exec_broadcast(cfd, resp, user, pass,"battleship", "battle1");
		printf("press enter to unicast...");
		getchar();
		exec_message(cfd, resp, user, pass,"battleship", "battle1", "mary");
		do {
			printf("press enter to remove topic...");
			getchar();
		}
		while (exec_remove_topic(cfd, resp,  user, pass, "battleship", "battle1")!= STATUS_OK);
		
		exec_remove_theme(cfd, resp,  user, pass, "battleship");
		exec_unregist(cfd, resp, user, pass);
	}
	else {
		do {
			printf("press enter to leave topic...");
			getchar();
		}
		while (exec_leave_topic(cfd, resp,  user, pass,"battleship", "battle1")!= STATUS_OK);
		
		exec_unregist(cfd, resp, user, pass);
		
	}
	
	

	
	printf("press enter to continue...");
	getchar();
	fclose(resp);
	close(cfd);
  	
	printf("client done!\n");
	return 0;
}
