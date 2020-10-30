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

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../include/reg_service2.h"

#define REG_SERVER_ADDR "127.0.0.1"
 
  
pthread_t thread;

int create_client_socket() {
	return socket(AF_INET, SOCK_STREAM, 0); 
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



int exec_stop(int socket, FILE *resp ) {
	char cmd[128];
	
	printf("Execute stop server!\n");
	int cmd_size = sprintf(cmd, "STOP\n\n");
	write(socket, cmd, cmd_size);
	
 
	
	return show_response_status(resp);
	
}
 
  
int main(int argc, char *argv[]) {
	 
 
	struct sockaddr_in srv_addr;
	int cfd;
	 
	if ((cfd = create_client_socket()) < 0) {
		fprintf(stderr, "error creating socket\n");
		return 1;
	}
	 
	
	/* Construct server address, and make the connection */
	bzero(&srv_addr, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr(REG_SERVER_ADDR);
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
		
	 
	/*
	if (exec_regist(cfd, resp, "stopper", "stoper_pass", 50000) != STATUS_OK) return 1;
	
	if (exec_create_theme(cfd, resp,  "mary", "mary_pass", "battleship") != STATUS_OK) return 1;
 	 
	if (exec_create_topic(cfd, resp, "mary", "mary_pass", "battleship", "battle1", 45000) != STATUS_OK) return 1;
	*/
	if (exec_stop(cfd, resp) != STATUS_OK) return 1;
 	

	fclose(resp);
	close(cfd);
  	
	printf("client done!\n");
	return 0;
}
