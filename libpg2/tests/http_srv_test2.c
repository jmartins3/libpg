#include "http_srv.h"

void staticHandler0(HttpRequest req,  HttpResponse resp) {
	const char *ip = http_req_addr(req);
	unsigned int port = http_req_port(req);
			
	http_resp_app_txt(resp, "Hello ");
	 
	http_resp_app_txt(resp, ip);
	http_resp_app_txt(resp, ":");
	http_resp_app_num(resp, port);
	http_resp_app_txt(resp, "!<br>");
	http_resp_app_txt(resp, "this is the response for your ");
	http_resp_app_txt(resp, http_req_method(req));
	http_resp_app_txt(resp, " request for ");
	http_resp_app_txt(resp, http_req_path(req));
	http_resp_set_status(resp, ok);
	http_resp_app_head(resp, "Content-Type", "text/html");
}
