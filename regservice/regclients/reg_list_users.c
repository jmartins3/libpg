/*
 * implements an echo client using a stream IP socket 
 */

 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "reg_client_utils.h"
 

	
int main(int argc, char *argv[]) {
	 
 
	char *server_ip_addr= get_server_ip();

	connection_t *cn;

	if ((cn = connect_to(server_ip_addr)) == NULL ) {
		fprintf(stderr, "error connection to server\n");
		return 1;
	}
 	 
	exec_list_users(cn);
 	
	cn_close(cn);
	return 0;
}

 
