/*--------------------------------------------------------
 * Gives a simple file front-end to cpp-netlib for C language
 * (client side)
 * 
 * JM, 2014
 * ------------------------------------------------------*/

#ifndef _HTTP_CLI_H
#define _HTTP_CLI_H

#ifndef CLIENT_IMPL

typedef  struct hr {} *HttpResponse;

#endif


#ifdef __cplusplus
extern "C" {
#endif
	


// C interface
HttpResponse http_do_request(const char *url);

void http_response_destroy(HttpResponse resp);

void http_response_show(HttpResponse resp);

const char * http_response_header(HttpResponse resp, const char *name);

const char * http_response_body(HttpResponse resp);


#ifdef __cplusplus		
}
#endif

#endif
