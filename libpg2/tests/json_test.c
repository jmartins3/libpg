#include "pg/json.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  
    FILE *file = NULL;
	JsonObj d = NULL;
	
    if (argc != 2) {
		printf("usage: %s <jsonfile>\n", argv[0]);
        return 1;
    }
    
   
	file = fopen(argv[1], "r");
	if (file == NULL) {
		 printf("Error opening %s\n" ,argv[1]);
		 return 1;
	}

	d = json_parse_file(file);
	if ( d== NULL) {
		printf("Bad json file!\n");
		return 1;
	}
	
	JsonObj results = json_get(d, "items");
	 
	if (results != NULL && json_is_array(results)) {
		for (int i = 0; i < json_array_size(results); ++i)
		{
			JsonObj curr = json_array_at(results, i);
			JsonObj volumeInfo = json_get(curr, "volumeInfo");
			JsonObj title = json_get(volumeInfo, "title");
			JsonObj publisher = json_get(volumeInfo, "publisher");
		 
			printf("%2d: %s\n", i+1, json_text(title));
			printf("\t\t%s\n",  json_text(publisher)); 
		}	 					 
	}
 
	json_close(d);
    
	 
	if (file != NULL)
		fclose(file);	

    return 0;
}
