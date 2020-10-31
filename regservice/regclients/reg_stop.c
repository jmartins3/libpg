/*
 * stop the regist server
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "reg_client_utils.h"
 

int main(int argc, char *argv[]) {
 
	char *server_ip_addr = getenv("REG_SERVER_IP");

	connection_t *cn;

	if ((cn = connect_to(server_ip_addr)) == NULL ) {
		fprintf(stderr, "error connection to server\n");
		return 1;
	}
 
				 
	exec_stop(cn);
 	
	cn_close(cn);
	return 0;
}


  
