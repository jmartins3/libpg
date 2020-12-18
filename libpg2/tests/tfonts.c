#include "pg/pglib.h"

#define WINDOW_HEIGHT 768
#define WINDOW_WIDTH 1900

#define FILL_COLOR	c_black

#define BACK_COLOR c_dgray
#define FORE_COLOR c_white


char letters[] = "abcdefghijklmnopqrstuvxwyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/()?!#$%&{}[].,;:\\-_*+|";

char letters1[] = "abcdefghijklmnopqrstuvxwyz";

int main() {
	
	graph_init2("Show Letters", WINDOW_WIDTH, WINDOW_HEIGHT);
	
	graph_rect(0,0, WINDOW_WIDTH, WINDOW_HEIGHT, FILL_COLOR, true);
	
	Size small_font = graph_font_size(SMALL_FONT);
	Size medium_font = graph_font_size(MEDIUM_FONT);
	Size large_font = graph_font_size(LARGE_FONT);
	
	int y = small_font.height +1;
	graph_text2(1, y, FORE_COLOR, BACK_COLOR, letters, SMALL_FONT);
	
	y+= medium_font.height + 1;
	graph_text2(1, y, FORE_COLOR, BACK_COLOR, letters, MEDIUM_FONT);
	
	y+= large_font.height + 1;
	graph_text2(1, y, FORE_COLOR, BACK_COLOR, letters, LARGE_FONT);
	
	
	y = WINDOW_HEIGHT/2;
	int x= 1;
	graph_text2(x, y, FORE_COLOR, BACK_COLOR, letters1, SMALL_FONT);
	x+= graph_text_size(letters1, SMALL_FONT).width;
	graph_text2(x, y, FORE_COLOR, BACK_COLOR, letters1, MEDIUM_FONT);
	x+= graph_text_size(letters1, MEDIUM_FONT).width;
	graph_text2(x, y, FORE_COLOR, BACK_COLOR, letters1, LARGE_FONT);
	
	
	y = WINDOW_HEIGHT-2;
	graph_text2(1, y, FORE_COLOR, BACK_COLOR, letters, SMALL_FONT);
	
	y-= small_font.height + 1;
	graph_text2(1, y, FORE_COLOR, BACK_COLOR, letters, MEDIUM_FONT);
	
	y-= medium_font.height + 1;
	graph_text2(1, y, FORE_COLOR, BACK_COLOR, letters, LARGE_FONT);
	
	
	graph_start();
	return 0;
	
}
