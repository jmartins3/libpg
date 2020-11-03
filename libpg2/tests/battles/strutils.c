#include "strutils.h"

#include <string.h>
#include <ctype.h>

bool is_space(int c) {
	return c == ' ' || c == '\t' ;
}

bool is_valid_char(char c, enum char_type type) {
	switch(type) {
		case Letters: return isalpha(c);
		case Digits:  return isdigit(c);
		case All: 	return !isspace(c);
		default: return false;
	}
}

int str_to_num(const char str[]) {
	int num = 0;
	for(int i= 0; str[i]!= 0; ++i) {
		if (!isdigit(str[i])) return -1;
		num = num*10 + (str[i] - '0');
	}
	return num;
}

/**
 * extracts a word from the line and returns the new start position
 */
int str_next(const char line[], int start, char word[], int max_size, enum char_type type) {
	int i= start;
	
	// jump separators
	while(line[i] != '\n' && is_space(line[i])) ++i;
	
	// get the line
	char c= line[i];
	int p = 0;
	while( p < max_size-1 &&  c != LF && !is_space(c) && is_valid_char(c,type)) {
		word[p++] = c;
		++i;
		c = line[i];
	};
	word[p]=0;
	
	// if no word exists or word was cutted, fail the operation
	if (p == 0 || (p == max_size-1 && is_valid_char(c,type))) return -1;
	else return i;
}

/*
 * extracts a number from a string and returns the new start position from the string
 */
int str_next_int(const char line[], int start, int *num) {
	char str_num[20];
	int ns;
	
	if ((ns = str_next(line, start, str_num, 20, Digits)) == -1) return -1;
	
	*num = str_to_num(str_num);
	
	if (*num == -1) return -1;
	return ns;
	
}

/*
 * extracts a number from a string and returns the new start position from the string
 */
int str_next_char(const char line[], int start, char *c) {
	char str_c[2];
	int ns;
	
	if ((ns = str_next(line, start, str_c, 2, All)) == -1) return -1;
	
	*c = str_c[0];
	 
	return ns;
	
}

