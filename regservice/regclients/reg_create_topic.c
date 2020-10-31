/*
 * create a regist user
 */


 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "reg_client_utils.h"
 
 
	
int main(int argc, char *argv[]) {
	char *theme;
	char *topic;
	char *user;
	char pass[64];
	
	if (argc != 4) {
		printf("usage: reg_create_topic <theme> <topic> <user>\n");
		return 1; 
	}
	
	theme = argv[1];
	topic = argv[2];
	user = argv[3];
	get_passwd(user,pass);
	
 
	char *server_ip_addr = getenv("REG_SERVER_IP");

	connection_t *cn;

	if ((cn = connect_to(server_ip_addr)) == NULL ) {
		fprintf(stderr, "error connection to server\n");
		return 1;
	}
 
				 
	exec_create_topic(cn, user, pass, theme, topic, MSG_PORT);
 	
	cn_close(cn);
	return 0;
}
