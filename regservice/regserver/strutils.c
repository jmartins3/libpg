#include "strutils.h"

bool is_space(int c) {
	return c == ' ' || c == '\t' ;
}

int str_next(const char text[], int start, char str[], int max_size) {
	int i= start;
	while(text[i] != LF && is_space(text[i])) ++i;
	
	char c= text[i];
	int p = 0;
	while( p < max_size-1 &&  c != LF && !is_space(c) ) {
		str[p++] = c;
		++i;
		c = text[i];
	};
	str[p]=0;
	
	if (p == 0 || (p == max_size-1 && (c != LF || !is_space(c)))) return -1;
	else return i;
}

bool check_line_termination(const char *line, int i) {
	while (line[i] != LF) 
		if (!is_space(line[i++])) return false;
	return true;
}

bool check_empty_line(const char *line) {
	return check_line_termination(line, 0);
}
