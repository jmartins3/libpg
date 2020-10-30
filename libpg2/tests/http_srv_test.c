#include "pg/http_srv.h"
#include <stdio.h>



void helloHandler(HttpRequest req,  HttpResponse resp) {
	char answer_template[] = "<html>" 
								"<body>"
									"<h1> Hello %s from server! </h1>"
									"<h2> Your request was: %s </h2>"
								 "</body>" 
							 "<html>";
	char buffer[512];
	
	sprintf(buffer, answer_template,  http_req_addr(req), http_req_path(req));
	 		
	http_resp_add_txt(resp, buffer);
	http_resp_add_header(resp, "Content-Type", "text/html");
	http_resp_set_status(resp, ok);
}


int main(int argc, char * argv[]) {
    if (argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }
   
    HttpServer srv = http_server_creat(argv[1]);
    http_server_add_handler(srv, "/", helloHandler);
    http_server_start(srv);
    return 0;
}





