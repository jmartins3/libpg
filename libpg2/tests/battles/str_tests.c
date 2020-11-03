#include <stdio.h>

#include "strutils.h"


int main() {
	const char line[] = "hello, 234 or 435?";
	int start = 0;
	
	char c1 ='?', c2='?';
	char word1[10] = "?", word2[3] ="?";
	int num1=-1, num2=-1;
	
	start = str_next(line, start, word1, 10, Letters);
	if (start!= -1)  
		start = str_next_char(line, start, &c1);
	if (start!= -1)  
		start = str_next_int(line, start, &num1);
	if (start!= -1)  
		start = str_next(line, start, word2, 3, Letters);
	if (start!= -1)  
		start = str_next_int(line, start, &num2);
	if (start!= -1)  
		start = str_next_char(line, start, &c2);
	 
	printf("word1='%s'\n", word1);
	
	printf("word2='%s'\n", word2);
	
	printf("c1='%c'\n", c1);
	printf("c2='%c'\n", c2);

	
	printf("num1=%d\n", num1);
	printf("num2=%d\n", num2);
	
	  
	return 0;
	
}
