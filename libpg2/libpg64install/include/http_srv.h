/*--------------------------------------------------------
 * Gives a simple file front-end to cpp-netlib for C language
 * (server side)
 * 
 * JM, 2014
 * ------------------------------------------------------*/

#ifndef SERVER_IMPL

typedef struct HttpReq  {} *HttpRequest;
typedef struct HttpResp {} *HttpResponse;
typedef struct HttpSrv  {} *HttpServer;

typedef void (*HttpHandler)(const HttpRequest req, HttpResponse resp);

 enum status_type {
            continue_http = 100,
            switching_protocols = 101,
            ok = 200,
            created = 201,
            accepted = 202,
            non_authoritative_information = 203,
            no_content = 204,
            reset_content = 205,
            partial_content = 206,
            multiple_choices = 300,
            moved_permanently = 301,
            moved_temporarily = 302,    ///< \deprecated Not HTTP standard
            found = 302,
            see_other = 303,
            not_modified = 304,
            use_proxy = 305,
            temporary_redirect = 307,
            bad_request = 400,
            unauthorized = 401,
            payment_required = 402,
            forbidden = 403,
            not_found = 404,
            not_supported = 405,        ///< \deprecated Not HTTP standard
            method_not_allowed = 405,
            not_acceptable = 406,
            proxy_authentication_required = 407,
            request_timeout = 408,
            conflict = 409,
            gone = 410,
            length_required = 411,
            precondition_failed = 412,
            request_entity_too_large = 413,
            request_uri_too_large = 414,
            unsupported_media_type = 415,
            unsatisfiable_range = 416,   ///< \deprecated Not HTTP standard
            requested_range_not_satisfiable = 416,
            expectation_failed = 417,
            precondition_required = 428,
            too_many_requests = 429,
            request_header_fields_too_large = 431,
            internal_server_error = 500,
            not_implemented = 501,
            bad_gateway = 502,
            service_unavailable = 503,
            gateway_timeout = 504,
            http_version_not_supported = 505,
            space_unavailable = 507,
            network_authentication_required = 511
        } status;
        
typedef enum status_type status_type;

#endif


#ifdef __cplusplus 
extern "C" {
#endif


/* server response */

// add text to the response
void http_resp_add_txt(HttpResponse resp, const char *text);

// add an int to the response
void http_resp_add_num(HttpResponse resp, int val);

// set response header
void http_resp_add_header(HttpResponse resp, const char *name, const char *value);

// set response status
void http_resp_set_status(HttpResponse resp, status_type status);

/* server initialization */

// create a server
HttpServer http_server_creat(const char * port);

// add a request handler for the server
void http_server_add_handler(HttpServer srv, const char *pat, HttpHandler handler);

// start the server!
void http_server_start(HttpServer srv);


/* request from client */

// get the request ip address 
const char *http_req_addr(HttpRequest req);

// get the request port
int http_req_port(HttpRequest req);

// get the request path
const char *http_req_path(HttpRequest req);

// get the request method
const char *http_req_method(HttpRequest req);

#ifdef __cplusplus 
}
#endif
