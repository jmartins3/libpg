/*-------------------------------------------------------
  C front/end for rapidjson parser

--------------------------------------------------------*/
 

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/filestream.h"
#include <iostream>
#include <stdio.h>

int main(int argc, char *argv[]) {
	
    using namespace rapidjson;
  
    FILE *file = NULL;

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << "<jsonfile>" << std::endl;
        return 1;
    }
    try {
		file = fopen(argv[1], "r");
		if (file == NULL) {
			  std::cout << "Error opening " << argv[1]  << std::endl;
			  return 1;
		}
	
        FileStream jsonFile(file);

        Document &d = *new Document();
        if (!d.ParseStream<0>(jsonFile).HasParseError()) {
		 
			std::cerr << "json correctly parsed!\n";
			if (d.IsObject())
				std::cerr << "document is an object!\n";
            const Value &results = d["items"];
            std::cerr << "json Value exists?";
            if (results.IsNull()) {
              std::cerr << "No!" << std::endl;
              const Value &errors = d["errors"];
              if (!errors.IsNull()) {
				    std::cerr << "errors type is " <<  errors.GetType() << std::endl;
				  if (errors.IsArray()) {
					  std::cerr << "errors is an array!" << std::endl;
					  const Value &message =  errors[(SizeType)0];
					  std::cerr << "message is: " <<  message["message"].GetString() << std::endl;
					  
					
					  std::cerr << "code  is " <<  message["code"].GetInt() << std::endl;
					
					 
				  } 
			   }
            } 
            else {
				 std::cerr << "Yes!" << std::endl;
				for (SizeType i = 0; i < results.Size(); ++i)
				{
					const Value &volumeInfo = results[i]["volumeInfo"];
					const Value &title = volumeInfo["title"];
					const Value &publisher = volumeInfo["publisher"];
					if (publisher.IsNull())
						std::cerr << i << ": " << "From: " << title.GetString() << std::endl << std::endl;
					else 
						std::cerr << i << ": " << "From: ""From: " << title.GetString() << std::endl
								  << "  " << publisher.GetString() << std::endl
								  << std::endl;
				}
			}
			delete &d;
        }
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
	
	if (file != NULL)
		fclose(file);	
    return 0;
}
