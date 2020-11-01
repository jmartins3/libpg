
/*-----------------------------------------------------------
 * This module supports an event-driven programming style
 * for simple games
 *----------------------------------------------------------*/


#define LIBPG_IMPL

#include 	<sys/times.h>
#include 	<map>
#include 	"../include/graphics.h"
#include 	<stdbool.h>
#include 	<signal.h>
#include 	"../include/audio.h"
#include  "../include/socket_events.h"

using namespace std;

/*-------------------------------------
 * Globals
 *------------------------------------*/

#define TIMEBASE		25	// 10 ms
#define NTICKSREPEAT 	6	// 150 ms



static map<uint, KeyEvent> pressedKeys;


// handlers defined for three event types 
static KeyEventHandler keyHandler;
static MouseEventHandler mouseHandler;
static TimerEventHandler timerHandler;

// SDL timer allocated
static SDL_TimerID my_timer_id;


// for repeat keyboard events and timer event processing 
static int nTicks;
static int currTick, currTickRepeat;


static Uint8 mouse_buttons;

static Uint8 mouse_state;

static void convertKeyEvent(SDL_KeyboardEvent *ksdl, KeyEvent *ke) {
	ke->state = (ksdl->state == SDL_PRESSED) ? KEY_PRESSED : KEY_RELEASED;
	ke->keysym = SDL_GetKeyFromScancode(ksdl->keysym.scancode);
	ke->keyscan = ksdl->keysym.scancode;
	strncpy(ke->keyName,SDL_GetKeyName(ke->keysym),MAX_NAME-1); 
	ke->keyName[MAX_NAME-1]=0;

}


static void convertMouseMotionEvent(SDL_MouseMotionEvent *msdl, MouseEvent *me) {
	me->type = MOUSE_MOTION_EVENT;
	 
	
	me->x= msdl->x;
	me->y= msdl->y;
	
	if ( mouse_buttons == SDL_BUTTON_LEFT) me->button = BUTTON_LEFT;
	else if ( mouse_buttons ==SDL_BUTTON_RIGHT) me->button = BUTTON_RIGHT;
	else me->button =0;
	
	 
	me->state = mouse_state == SDL_PRESSED ? BUTTON_PRESSED : 0;
	
	me->dx= msdl->xrel;
	me->dy= msdl->yrel; 
	
	
}


#define MAX_CLICK_PERIOD	20
static MouseEvent lastMe;
static clock_t lastTicks;
static bool auto_repeat;

static void convertMouseButtonEvent(SDL_MouseButtonEvent *msdl, MouseEvent *me) {
	clock_t currTicks = times(NULL);
	
	me->type = MOUSE_BUTTON_EVENT;
	me->state = msdl->state == SDL_PRESSED ? BUTTON_PRESSED : BUTTON_RELEASED;
	
	if (me->state  == BUTTON_PRESSED) {
		mouse_buttons = msdl->button;
		mouse_state = msdl->state;
	}
	else {
		mouse_buttons = 0;
		mouse_state = 0;
	}
	 
	if ( msdl->button == SDL_BUTTON_LEFT) me->button = BUTTON_LEFT;
	else if ( msdl->button == SDL_BUTTON_RIGHT) me->button = BUTTON_RIGHT;
	else  me->button = 0; 
	
	if (lastMe.state == BUTTON_PRESSED && me->state == BUTTON_RELEASED &&
	    lastMe.button == me->button && currTicks - lastTicks < MAX_CLICK_PERIOD) {
		// produce a click instead of a release state
		me->state = BUTTON_CLICK;
	}
	
	lastMe = *me;
	lastTicks = currTicks;
	
	me->x= msdl->x;
	me->y= msdl->y;
	
	me->dx= 0;
	me->dy= 0; 
}


// this callback is running on a separate thread
// generates a user event to processe the base time in 
// the proper thread
static uint my_callbackfunc(Uint32 interval, void *param)
{
    SDL_Event event;
    
    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */

	
    event.user.type = SDL_USEREVENT;
    event.user.code = USER_BASETIME_EVENT;
    event.user.data1 = NULL;
    event.user.data2 = NULL;

    event.type = SDL_USEREVENT;
    
    SDL_PushEvent(&event);
    return(interval);
}

void graph_regist_key_handler(KeyEventHandler ke) {
	keyHandler = ke;
}


void graph_regist_mouse_handler(MouseEventHandler me) {
	mouseHandler = me;
}


void graph_regist_timer_handler(TimerEventHandler te, uint period) {
	nTicks = (period-1) / TIMEBASE +1 ;  /* To round it down to the nearest 10 ms */
	currTick = 0;
	currTickRepeat = 0;
	timerHandler = te;
}

void graph_set_auto_repeat_off() {
	auto_repeat = false;
	currTickRepeat = 0;
}

void graph_set_auto_repeat_on() {
	auto_repeat = true;
}

extern "C" {
void onTick();
	 

bool onEnd();

 
}

void timebase_handler() {
	
	if (nTicks != 0 && ++currTick == nTicks) {
		currTick = 0;
		onTick();
		if (timerHandler != NULL)
				timerHandler();
	}
	
	if (keyHandler!= NULL && auto_repeat && ++currTickRepeat == NTICKSREPEAT   ) {
		currTickRepeat = 0;
		for (map<uint,KeyEvent>::iterator it = pressedKeys.begin(); 
			it != pressedKeys.end(); ++it)
			keyHandler(it->second);
	}
 
	// audio
	audio_process();

}

void timebase_regist() {
	if (nTicks == 0) nTicks = (1000-1) / TIMEBASE +1;
	// if (timerHandler != NULL || keyHandler != NULL)
		my_timer_id = SDL_AddTimer(TIMEBASE, my_callbackfunc, NULL);
}

static bool process_user_event(SDL_Event *event) {
	if (event->user.code == USER_BASETIME_EVENT) {
		timebase_handler();
		
	}
	else if (event->user.code == END_SESSION_EVENT) {
		audio_close();
		SDL_Quit();
		return false;
	}
	else if (event->user.code == REQUEST_RESPONSE_EVENT) {
		msg_request_t *msg = (msg_request_t*) event->user.data1;
		
		if (msg != NULL) {
			// adjust status 
			if (msg->status == 0) { // no comm error
				sscanf(msg->resp, "%d", &msg->status);
			}
			msg->on_response(msg->status, msg->resp);
			if (msg->resp != NULL) free(msg->resp);
			if (msg->cmd != NULL) free(msg->cmd);
			free(msg);
		}
	}
	else if (event->user.code == NOTIFICATION_EVENT) {
		session_t session = (session_t) event->user.data1;
		if (session != NULL && session->notification != null) {
			// TODO
			char *line2 = session->notification;
			printf("msg received:%s\n", session->notification);
			while(*line2 !='\n') ++line2;
			++line2;
			char *sender = line2;
			while(*line2 !=' ') ++line2;
			*line2++ = 0;
			while(*line2 !='\n') ++line2;
			*line2++=0;
		}
		 
		session->on_msg(sender, line2);
		free(session->notification);
		session->notification = NULL;
		 
	}
	return true;
					
}


/*-------------------------------------
 *  Properly refresh window
 *------------------------------------*/
void  graph_start() {
	SDL_Event event;
	KeyEvent keyEvent;
	MouseEvent mouseEvent;

	map<uint,KeyEvent>::iterator it;
	
	
	timebase_regist();
	
	while ( SDL_WaitEvent(&event)  && !__done) {
		switch (event.type) {
		 	case SDL_USEREVENT:
				if (!process_user_event(&event)) return;
				break;
			case SDL_KEYDOWN:	
				if (keyHandler != NULL) {
					convertKeyEvent((SDL_KeyboardEvent*) &event, &keyEvent);
					if ((it = pressedKeys.find(keyEvent.keyscan)) == pressedKeys.end()) {
						pressedKeys[keyEvent.keyscan] = keyEvent;
						keyHandler(keyEvent);
					}
				}
				break;
			case SDL_KEYUP:
				if (keyHandler != NULL) {
					convertKeyEvent((SDL_KeyboardEvent*) &event, &keyEvent);
					if ((it = pressedKeys.find(keyEvent.keyscan)) != pressedKeys.end()) {
						pressedKeys.erase(it);
						keyHandler(keyEvent);
					}
				}
				break;
					
			case SDL_MOUSEMOTION:
				if (mouseHandler != NULL) {
					convertMouseMotionEvent((SDL_MouseMotionEvent*) &event, &mouseEvent);
					mouseHandler(mouseEvent);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				if (mouseHandler != NULL) {
					convertMouseButtonEvent((SDL_MouseButtonEvent*) &event, &mouseEvent);
					mouseHandler(mouseEvent);
				}
				break;
			case SDL_QUIT:
				if (onEnd()) {
					SDL_Quit();
					return;
				}
				break;
			 
			default:
				break;
			
		}
		
		graph_refresh();
	 
	}
	
} 

 	 


