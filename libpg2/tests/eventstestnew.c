/*---------------------------------------------------------
 libgraph events tester 
 
 Jorge Martins, 2014
 ---------------------------------------------------------*/
 
 
#include <stdio.h>


#include "pg/graphics.h"
#include "pg/events.h"

#define GRAPH_WIDTH 1024
#define GRAPH_HEIGHT 768

/* handlers */
 
	
void myKeyEventHandler(KeyEvent ke) {
	printf("KeyEvent! (%d, %d, %d, %s)\n", ke.state, ke.keyscan, ke.keysym, ke.keyName); 	
    if (!strcmp(ke.keyName, "S"))
		sound_stop();
	else  if (!strcmp(ke.keyName, "P"))
		sound_pause();
	else  if (!strcmp(ke.keyName, "R"))
		sound_resume();
}

void myMouseEventHandler(MouseEvent me) {
	if (me.type == MOUSE_BUTTON_EVENT)
		printf("Mouse Button Event! (%d,%d,%s,%d)\n", 
				me.x, me.y, me.button == BUTTON_LEFT ? "Left": 
				   (me.button == BUTTON_RIGHT ? "Right" : "None"), 
				   me.state ); 
	else  if (me.type == MOUSE_MOTION_EVENT)
		printf("Mouse Motion Event! (%d,%d,%s,%d)\n", 
				me.x, me.y, me.button == BUTTON_LEFT ? "Left": 
				(me.button == BUTTON_RIGHT ? "Right" : "None"),
				 me.state ); 			
}


void myTimerEventHandler() {
	printf("TimerEvent!\n");  
}



int  main() {
	// intialize the graph engine
	if (graph_init() == -1) {
		printf("Error initializing graph library!\n");
		return (1);
	}
	 
	// regist event handlers
	
	// "myKeyEventHandler" function will process the keyboard events
	graph_regist_key_handler(myKeyEventHandler);
	
	// "myTimerEventHandler" function will process the timer events
	//graph_regist_timer_handler(myTimerEventHandler, 1000);
	
	// "myMouseEventHandler" function will process the mouse events
	graph_regist_mouse_handler(myMouseEventHandler);
	
	graph_image("media/DSC_0033.JPG", 0, 0, GRAPH_WIDTH, GRAPH_HEIGHT);
	
	sound_play("media/videogames.wav");
	// start the graph engine	
	graph_start();
	
	return 0;
}
