#include "strutils.h"

#include <string.h>
#include <ctype.h>



/**
 * dado o caracter "c", a função retorna true se for do tipo indicado,
 * false caso contrário
 */
bool is_valid_char(char c, enum char_type type) {
	switch(type) {
		case Letters: return isalpha(c);
		case Digits:  return isdigit(c);
		case Other: return ispunct(c);
		case All:  return c != 0 && !isspace(c);
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
	while(isblank(line[i])) ++i;
	
	// get the line
	char c= line[i];
	int p = 0;
	while( p < max_size-1 && c != 0 && is_valid_char(c,type)) {
		word[p++] = c;
		++i;
		c = line[i];
	};
	word[p]=0;
	
	// if no word exists or word was cutted, fail the operation
	if (p == 0 || (p == max_size-1 && c!= 0 && is_valid_char(c,type))) return -1;
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
	int i= start;
	 
	// jump separators
	while(isblank(line[i])) ++i;
	
	if (line[i] == LF || line[i] == 0) return -1;
	
	*c = line[i++];
	 
	return i;
	
}

/**
 * Recolhe, para a string "line", uma linha da string "text" a partir da posição "start",
 * com todos os caracteres presentes em "text", incluindo separadores. 
 * 
 * A recolha termina quando for encontrado em "text" o caracter '\n'  ou o terminador '\0'.
 * A função retorna a posição de "text" a seguir ao caracter '\n' ou a posição do terminador '\0';
 * Caso seja excedida a capacidade de "line" indicada em "line_capacity" os caracteres sobrantes da linha
 * em "text" deverão ser descartados.
 */
int str_next_line(const char text[], int start, char line[], int line_capacity) {
	int i= start, p = 0;
	
	int c = text[i];
	while( c != '\n' && c != 0 && p < line_capacity -1 ) {
		line[p++] = c;
		c = text[++i];
	}
	
	while( c != '\n' && c != 0) {
		// descartar 
		c= text[++i];
	}
	line[p] = 0;
	return (c == 0) ? i : i +1;
}

int str_next_word(const char line[], int start, char word[], int max_size) {
	return str_next(line, start, word, max_size, Letters);
}
