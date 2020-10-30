#include <stdio.h>

#include "squash.h"

#include "pg/graphics.h"

#include "counter.h"


Racket racket;
Counter counter;
Ball ball;

RGB c_brown;
RGB c_board_floor;

 
/* handlers */

	
void myKeyEventHandler(KeyEvent ke) {
	if (ke.state == KEY_PRESSED)
		racket_move(&racket, ke.keysym);		
}

static int suspend_ball;
static int end;


static void end_game() {
	end = true;
}

void loosePoint() {
	suspend_ball = 30;
	ball_setPos(&ball, 250, BORDER_SIZE+BOARD_TOP+2*BALL_RADIUS+1, false);
	if (Counter_dec(&counter) == 0)
		end_game();
		
}

void myTimerEventHandler() {
	if (end) 
		return;
	if (suspend_ball > 0)
		suspend_ball--;
	else
		ball_move(&ball, &racket);
}

RGB floor_color() {
	return c_board_floor;
}

RGB border_color() {
	return c_brown;
}

void draw_border() {
	
	graph_rect(0,0, GRAPH_WIDTH, BOARD_TOP, c_brown, true);
	graph_rect(0,BOARD_TOP, BORDER_SIZE,  BOARD_BOTTOM-BOARD_TOP+1, c_brown, true);
	graph_rect(BOARD_RIGHT, BOARD_TOP, BORDER_SIZE, BOARD_BOTTOM-BOARD_TOP+1, c_brown, true);
	graph_rect(0,BOARD_BOTTOM, GRAPH_WIDTH, BORDER_SIZE, c_brown, true);
}

void draw_board() {
	c_brown = graph_rgb(91,25,8);
	c_board_floor = graph_rgb(39, 140, 28);
	graph_rect(0,0, GRAPH_WIDTH, GRAPH_HEIGHT, c_board_floor, true);
	
	 
	draw_border();
	
	// draw counter
	Counter_init(&counter, 19, 300, 5, 70, 60);	
}


 
int main() {
	if (graph_init() == -1) {
		printf("Error initializing events library!\n");
		return (1);
	}
	 
	// regist callbacks
	uint kup=SDLK_UP;
	printf("SDLK_DOWN=%d\n",  SDLK_DOWN);
	printf("SDLK_UP=%d\n", kup);
    
	graph_regist_key_handler(myKeyEventHandler);
	 
	graph_regist_timer_handler(myTimerEventHandler, 50);
	draw_board();
	
	ball_creat(&ball, BALL_START_X, BALL_START_Y);
	racket_creat(&racket, RACKET_X, RACKET_START_Y);
	
	graph_start();
	
}
