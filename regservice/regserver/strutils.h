#pragma once

#include <stdbool.h>


#define LF '\n'
#define CR '\r'

bool is_space(int c);
	 

int str_next(const char text[], int start, char str[], int max_size);

bool check_line_termination(const char *line, int i);

bool check_empty_line(const char *line);
