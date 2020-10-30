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
 	
	fclose(resp);
	close(cfd);
 
	return 0;
}
