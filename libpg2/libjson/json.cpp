/*-------------------------------------------------------------
   json reader C api
 ---------------------------------------------------------------*/
 
#include "json_impl.h"
 
using namespace rapidjson;

 
JsonObj json_parse_str(const char *str) {
	try {
		Document *doc = new Document();
		doc->Parse<0>(str);
		if (doc->HasParseError()) {
			delete doc;
			return NULL;
		}
		return doc;	
		
	}
	catch (std::exception &e) {
        return NULL;
    }
}

JsonObj json_parse_file(FILE *file) {
	try {
		FileStream jsonFile(file);
		Document *doc = new Document();
		doc->ParseStream<0>(jsonFile);
		if (doc->HasParseError()) {
			delete doc;
			return NULL;
		}
		return doc;	
		
	}
	catch (std::exception &e) {
        return NULL;
    }
}	

JsonObj json_get(JsonObj obj, const char *fieldName) {
	try {
		Value &o = (*obj)[fieldName];
		if (o.IsNull()) return NULL;
		return &o;
	}
	catch (std::exception &e) {
        return NULL;
    }
}

JsonString json_text(JsonObj obj) {
	if (obj == NULL) return "";
	return obj->GetString();
}

bool json_is_array(JsonObj obj) {
	if (obj == NULL) return false;
	return obj->IsArray();
}

int json_array_size(JsonObj obj) {
	if (!json_is_array(obj)) return 0;
	return (int) obj->Size();
}		

JsonObj json_array_at(JsonObj obj, int idx) {
	if (!json_is_array(obj)) return NULL;
	return  &(*obj)[(SizeType) idx];
}	

void json_close(JsonObj doc) {
	try {
		// No virtual destructor
		// Must do the cast!
		delete (Document *) doc;
	 
	}
	catch (std::exception &e) {
       
    }
}
