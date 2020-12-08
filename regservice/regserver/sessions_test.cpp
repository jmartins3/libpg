
#include <stdio.h>
#include <cstring>
#include <map>
#include "activeusers.h"


using namespace std;

int main() {
	
	user_t john;
	strcpy(john.name, "john");
	user_t mary;
	
	strcpy(mary.name, "mary");
	static uv_loop_t loop;
	uv_tcp_t s1, s2;
	
	uv_loop_init(& loop);
	uv_tcp_init(&loop, &s1);
	uv_tcp_init(&loop, &s2);
	
	
	get_session(&s1, &john);
	get_session(&s2, &mary);
	
	map<uv_tcp_t*,user_session_t *>::iterator it ;
	for(it = sessions.begin(); it != sessions.end(); ++it) 
		printf("user: %s\n", it->second->user->name);


	return 0;
}
 
