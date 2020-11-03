#pragma once

#include <stdbool.h>


#define LF '\n'
#define CR '\r'


enum char_type { Letters, Digits, All };



bool is_space(int c);
 

bool is_valid_char(char c, enum char_type type);
	 

int str_to_num(const char str[]);
	 

/**
 * extracts a word from the line and returns the new start position
 */
int str_next(const char line[], int start, char word[], int max_size, enum char_type);
	 
	 
/*
 * extracts a number from a string and returns the new start position from the string
 */
int str_next_int(const char line[], int start, int *num);
	 

int str_next_char(const char line[], int start, char *c);
