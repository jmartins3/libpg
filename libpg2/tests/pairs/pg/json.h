/*-------------------------------------------------------------
   json reader C api
   
   JM, 2014
---------------------------------------------------------------*/
 
#ifndef _JSON_H
#define _JSON_H

#include <stdio.h> 
#ifndef JSON_IMPL

typedef struct JO {} *JsonObj;

#ifndef __cplusplus
#include "stdbool.h"
#endif

#endif

typedef const char *JsonString;


#ifdef __cplusplus
extern "C" {
#endif

// Get a json object from parsing str  
JsonObj json_parse_str(const char *str);

// Get a json object from parsing file 
JsonObj json_parse_file(FILE *file);

// get field with name "fieldName" from "obj"
JsonObj json_get(JsonObj obj, const char *fieldName);

// get text representation of "obj"	 
JsonString json_text(JsonObj obj);
	 
// check if "obj" is a json array
bool json_is_array(JsonObj obj);
	
// assuming that "obj" is a json array , return the correspondent size
// if it's not ana rray, return 0
int json_array_size(JsonObj obj);

// assuming that "obj" is a json array , return the json object at index "i".	 
// if it's not ana rray, return NULL
JsonObj json_array_at(JsonObj obj, int idx);
	 
// At the end must be called with original object parsed from string or file (doc)
void json_close(JsonObj doc);

#ifdef __cplusplus
}
#endif
	 

#endif
