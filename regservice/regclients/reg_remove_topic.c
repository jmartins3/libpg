/*
 * remove a topic
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
		printf("usage: reg_remove_topic <theme> <topic> <user>\n");
		return 1; 
	}
	
	user = argv[1];
	theme = argv[2];
	topic = argv[3];

	get_passwd(user,pass);
	
 
	char *server_ip_addr = getenv("REG_SERVER_IP");

	connection_t *cn;

	if ((cn = connect_to(server_ip_addr)) == NULL ) {
		fprintf(stderr, "error connection to server\n");
		return 1;
	}
 
				 
	exec_remove_topic(cn, user, pass, theme, topic);
 	
	cn_close(cn);
	return 0;
}
