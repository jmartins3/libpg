#include <stdio.h>
#include "pg/http_cli.h"
#include "pg/json.h"

int main(int argc, char *argv[]) {
	
	const char url_template[] = "https://www.googleapis.com/books/v1/volumes?q=%s";
	char line[256];
	
	do {
		printf("Pesquisa? ") ;
		if (gets(line) == NULL || line[0] == 0)
			break;
		
		char url[256];
		sprintf(url, url_template, line);
	
		HttpResponse resp = http_do_request(url);
	
		if (resp==NULL) 
			printf("error on getting http response!\n");
		else {
		 
			const JsonString body = http_response_body(resp);
				
			JsonObj d = NULL;

			d = json_parse_str(body);
			if ( d == NULL) {
				printf("Bad json file!\n");
			}
			else {
				JsonObj results = json_get(d, "items");
			  
				for (int i = 0; i < json_array_size(results); ++i)
				{
					JsonObj curr = json_array_at(results, i);
					JsonObj volumeInfo = json_get(curr, "volumeInfo");
					JsonObj title = json_get(volumeInfo, "title");
					JsonObj publisher = json_get(volumeInfo, "publisher");
				 
					printf("%2d: %s\n", i+1, json_text(title));
					printf("\t\t%s\n",  json_text(publisher)); 
				}	 					 
			 
				json_close(d);
			}
	 
			http_response_destroy(resp);
		}
	} while(1);
	return 0;
}
