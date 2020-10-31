/*
 * create a regist user
 */


 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "reg_client_utils.h"
 

	
int main(int argc, char *argv[]) {
	int number;
	char *user;
	char pass[64];
	
	if (argc != 3) {
		printf("usage: reg_create_user <number> <user>\n");
		return 1; 
	}
	
	number = atoi(argv[1]);
	user = argv[2];
	get_passwd(user,pass);
	
 
	char *server_ip_addr = getenv("REG_SERVER_IP");

	connection_t *cn;

	if ((cn = connect_to(server_ip_addr)) == NULL ) {
		fprintf(stderr, "error connection to server\n");
		return 1;
	}
 
				 
	exec_regist(cn, user, pass, number);
 	
	cn_close(cn);
	return 0;
}
