/*
 * implements an echo client using a stream IP socket 
 */

 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "reg_client_utils.h"
 

	
int main(int argc, char *argv[]) {
	 
	char *theme;
	
	if (argc != 2) {
		printf("usage: reg_list_topic <theme>\n");
		return 1;
	}
	theme = argv[1];
	
	char *server_ip_addr = getenv("REG_SERVER_IP");

	connection_t *cn;

	if ((cn = connect_to(server_ip_addr)) == NULL ) {
		fprintf(stderr, "error connection to server\n");
		return 1;
	}
 	 
	exec_list_topics(cn,ADMIN, ADMIN_PASS,  theme);
 	
	cn_close(cn);
	return 0;
}

 
