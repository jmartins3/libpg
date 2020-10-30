#include "boost/network/protocol/http/client.hpp"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include <iostream>

int main(int argc, char *argv[]) {
    using namespace boost::network;
    using namespace rapidjson;

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <query>" << std::endl;
        return 1;
    }

    try {
        http::client client;

        uri::uri base_uri("http://search.twitter.com/search.json");

        std::cout << "Searching Twitter for query: " << argv[1] << std::endl;
        uri::uri search;
        search << base_uri << uri::query("q", uri::encoded(argv[1]));
        http::client::request request(search);
        http::client::response response = client.get(request);

        Document d;
        if (!d.Parse<0>(response.body().c_str()).HasParseError()) {
		 
			std::cerr << "json correctly parsed!\n";
			if (d.IsObject())
				std::cerr << "document is an object!\n";
            const Value &results = d["results"];
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
				for (SizeType i = 0; i < results.Size(); ++i)
				{
					const Value &user = results[i]["from_user_name"];
					const Value &text = results[i]["text"];
					std::cerr << "From: " << user.GetString() << std::endl
							  << "  " << text.GetString() << std::endl
							  << std::endl;
				}
			}
        }
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
