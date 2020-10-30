#include <stdio.h>
#include <ctype.h>

#include "pg/graphics.h"
#include 	<map>

#include "pg/events.h"
//#include "utils.h"

// #include "components.h"

#define GRAPH_WIDTH 1400
#define GRAPH_HEIGHT 900



#define START_X 0
#define START_Y 100

#define ROW1_START 33
#define ROW1_END	80

#define ROW2_START (ROW1_END)
#define ROW2_END 127

using namespace std;

// globals

 

#define BLACK 0
#define WHITE 1

int font;
int counts[128][2];
static map<RGB, int> counts2[128];

static void count_colors(int x0, int y0, Size font_size, map<RGB, int> &colors) {
		for(int y= y0; y > y0 - font_size.height; --y) {
			for(int x= x0; x < x0 + font_size.width; ++x) {
				RGB clr = graph_get_pixel(x,y);
				colors[clr]++;
			}
		}
}


static void count_white_black(int x0, int y0, Size font_size, int *wc, int *bc ) {
		int cbl=0, cwl = 0;
		for(int y= y0; y > y0 - font_size.height; --y) {
			for(int x= x0; x < x0 + font_size.width; ++x) {
				RGB clr = graph_get_pixel(x,y);
				if (clr == c_white) cwl++;
				else cbl++;
			}
		}
		*wc = cwl; 
		*bc = cbl;
			
}

static void check_letters() {
	for(int i=ROW1_START; i < ROW2_END; ++i) {
		 if (counts[i][0] != 0 && counts[i][0] <= counts[i][1]) 
			 printf("%c: majority on foreground!\n", i);
	}
}

static void process_letters() {
	Size font_size = graph_font_size(font);
	int x= 0, y=100;
	
	graph_start_capture();
	for(int i=ROW1_START; i < ROW1_END; ++i) {
		count_colors(x, y, font_size, counts2[i]);
		x+=20;
	}
	y+= 50; x= 0;
	for(int i=ROW2_START; i < ROW2_END; ++i) {
		/*int wc=0, bc=0;
		count_white_black(x, y, font_size, &wc, &bc);
		counts[i][BLACK] = bc;
		counts[i][WHITE] = wc;
		*/
		count_colors(x, y, font_size, counts2[i]);
		x+=20;
	}
	graph_end_capture();
	
	check_letters();
}

static void show_color_counts(int car, map<RGB, int> &counts) {
	int maxCount = 0;
	RGB maxColor= c_black;
	int i=1;
	for (map<RGB,int>::iterator it = counts.begin(); 
			it != counts.end(); ++it) {
				if (car == 'N') {
					printf("count[%d]=%d\n", i++, it->second);
				}
				if (it->second > maxCount) {
					maxCount = it->second;
					maxColor = it->first;
				 }
				 			
	}
	
	printf("%c: total_colors=%d, max=rgb(%d,%d,%d)-%d\n",
			car, (int) counts.size(), rgb_red(maxColor), rgb_green(maxColor), rgb_blue(maxColor),
			maxCount);
			 
}

static void show_counts() {
	printf("CHAR\tWHITES\tBLACKS\n");
	for(int i=ROW1_START; i < ROW2_END; ++i) {
		 if (counts[i][0] != 0 || counts[i][1] != 0) 
			 printf("%c\t%d\t%d\n", i, counts[i][1], counts[i][0]);
	}
}

static void show_counts2() {
	 
	for(int i=ROW1_START; i < ROW2_END; ++i) {
		 if (counts2[i].size() != 0) 
			show_color_counts(i, counts2[i]); 
	}
}

static int processed;

void proc_key(KeyEvent ke) {
	if (ke.state == KEY_PRESSED && !processed) {
		process_letters();
		show_counts2();
		processed=1;
	}
}

void draw_letters() {
	Size font_size = graph_font_size(font);
	int x= 0, y=100;
	char text[] = {0,' ', 0};
	for(int i=ROW1_START; i < ROW1_END; ++i) {
		if (isprint(i)) {
			printf("%c\n", i);
			text[0] = i;
			graph_text(x, y, c_white, text, font);
			x+=font_size.width+1;
		}
	}
	
	//y+= font_size.height+1; 
	x= 0;
	for(int i=ROW2_START; i < ROW2_END; ++i) {
		if (isprint(i)) {
			printf("%c\n", i);
			text[0] = i;
			graph_text(x, y, c_white, text, font);
			x+= font_size.width+1;
		}
	}
}



void draw_letters2() {
	int x= 0, y=100;
	char text[128] = {0};
	int npos= 0;
	for(int i=ROW1_START; i < ROW1_END; ++i) {
		if (isprint(i)) { 
			text[npos++] = i;
		}
	}
	text[npos]=0;
	graph_text(x,y, c_white, text, font);
	//process_letters();
	//y+= font_size.height+1; 
	npos=0;
	for(int i=ROW2_START; i < ROW2_END; ++i) {
		if (isprint(i)) {
			text[npos++] = i;
		}
	}
	graph_text(x,y, c_white, text, font);

}

int main(int argc, char *argv[]) {
	counts2[0][c_white] = 0;
	counts2[0][c_black] = 0;
	
	printf("size(count2)=%d\n", (int) (counts2[0].size()));
	
	if (argc != 2) {
		printf("usage: show letters { s | m | l }\n");
		return 0;
	}
	switch(argv[1][0]) {
		case 's': case 'S' : font = SMALL_FONT;
							break;
		case 'm': case 'M' : font = MEDIUM_FONT;
							break;
		case 'l': case 'L' : font = LARGE_FONT;
							break;
		default:
			printf("bad font size!\n");
			return 0;
	}
		
 
	graph_init2(GRAPH_WIDTH, GRAPH_HEIGHT);
	graph_rect(0,0, GRAPH_WIDTH, GRAPH_HEIGHT, c_blue, true);
	
	
	draw_letters2();
	
	graph_regist_key_handler(proc_key);
	graph_start();
	 
	return 0;
}
