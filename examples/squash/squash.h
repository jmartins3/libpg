#ifndef _SQUASH_H
#define _SQUASH_H

#include <stdbool.h>
#include "pg/pglib.h"
 
#include "racket.h"
#include "ball.h"

#define GRAPH_WIDTH 800
#define GRAPH_HEIGHT 600

#define BORDER_SIZE 	10
#define BOARD_TOP		75
#define BOARD_BOTTOM	(GRAPH_HEIGHT - BORDER_SIZE)

#define BOARD_LEFT		BORDER_SIZE
#define BOARD_RIGHT		(GRAPH_WIDTH - BORDER_SIZE)

#define RACKET_X		60
#define	RACKET_START_Y	100

#define BALL_START_X	400
#define BALL_START_Y 	200


#define COUNTER_X (GRAPH_WIDTH /2 - 40)
#define COUNTER_Y 5
#define COUNTER_WIDTH 70
#define COUNTER_HEIGHT 60
#define COUNTER_INITIAL 19


RGB  border_color();
RGB  floor_color();
void loosePoint();

#endif
