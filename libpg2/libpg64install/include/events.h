#ifndef _EVENTS_H
#define _EVENTS_H

#ifdef LIBPG_IMPL
#include <SDL2/SDL.h>

extern SDL_Renderer *screen;


#endif

typedef unsigned short ushort;
typedef unsigned char byte;
typedef unsigned int uint;

/*
 *  communication events callback types
 */


typedef  void (*ResponseEventHandlerEx)(int status, const char response[], void * context);
typedef  void (*MsgEventHandlerEx)(const char msg[], void * context);



/*------------------------------------------
 * events callback registration
 * -----------------------------------------*/

/*
 * User Event codes
 */
 #define REQUEST_RESPONSE_EVENT	1
 #define END_SESSION_EVENT 		2
 #define USER_BASETIME_EVENT 	3
 #define NOTIFICATION_EVENT 	4

/* 
 * MOUSE Events constants 
 */

#define MOUSE_MOTION_EVENT 1
#define MOUSE_BUTTON_EVENT 2


#define BUTTON_LEFT		1
#define BUTTON_RIGHT	2


#define BUTTON_PRESSED	1
#define BUTTON_RELEASED	2
#define BUTTON_CLICK	4

typedef struct MouseEvent  {
	int type; 		/* mouse motion or mouse button event type */
	byte state;		/* button pressed or released */
	int button;		/* which button is pressed */
	ushort x, y;	/* coordinates of the mouse */
	short dx, dy;	/* delta move when mouse motion */
} MouseEvent;
























/* 
 * KEYBOARD Events constants 
 */

#define KEY_PRESSED 	0
#define KEY_RELEASED	1
#define MAX_NAME		30

/* Events state */

typedef struct KeyEvent {
	int state;					/* key pressed or released */ 
	uint keyscan;				/* key scancode */
	uint keysym;				/* key virtual code */
	char keyName[MAX_NAME];		/* key name */
} KeyEvent;




/*--------------------------------------------
 *  user interface callbacks 
 *-------------------------------------------*/

typedef void (*KeyEventHandler)(KeyEvent ke);
typedef void (*MouseEventHandler)(MouseEvent me);
typedef void (*TimerEventHandler)();


/*--------------------------------------------
 * publics
 *********************************************/

#ifdef __cplusplus 
extern "C" {
#endif



/*------------------------------------
 * regist the keyboard event handler
 *-----------------------------------*/
void graph_regist_key_handler(KeyEventHandler ke);


/*-------------------------------------
 * Regist the mouse event handler
 *------------------------------------*/	 
void graph_regist_mouse_handler(MouseEventHandler me);
 
/*----------------------------------------
 * Regist the timer event handler
 *---------------------------------------*/
void graph_regist_timer_handler(TimerEventHandler te, uint period);

	 
/*----------------------------------------
 * Key handler behaviour  
 *---------------------------------------*/
void graph_set_auto_repeat_off();
void graph_set_auto_repeat_on();

#ifdef __cplusplus 
}
#endif 

#endif
