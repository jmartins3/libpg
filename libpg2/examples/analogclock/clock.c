/*--------------------------------------------------------
  Relógio
  
  Jmartins, 2014
---------------------------------------------------------*/

#include <stdio.h>
#include <math.h>

#include "pg/pglib.h"
#include "analogclock.h"


Point hoursPos[12];
Point secsPos[60];
Point minutsPos[60];

int secs, minuts, hours;

void calculatePositions(Point positions[], int radius, int npos) {
	const double pi = 3.1415926;
	 
	Point p = { CX, CY - radius };
	
	double currDegreesAngle = 90;
	int stepDegrees= 360/npos;
	
	positions[0] = p;
	
	for (int i=npos-1; i > 0; --i) {
		currDegreesAngle += stepDegrees;
		// convert to radians
		double radAng = (currDegreesAngle * pi) / 180;
		positions[i].x = cos(radAng)*radius+ CX;
		positions[i].y = -sin(radAng)*radius+ CY;
	}
}


void drawInitialClock() {
	Point dotsSecsPos[60], dotsHoursPos[12];
	calculatePositions(dotsSecsPos, RADIUS, 60);
	calculatePositions(dotsHoursPos, RADIUS, 12);

	graph_rect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT, c_white, true);
	graph_line(CX, CY, CX, CY - RADIUS-2, c_blue);

	for (int i=0; i < 60; ++i) {
		 graph_pixel(dotsSecsPos[i].x, dotsSecsPos[i].y, c_black);
	}
	
	for (int i=0; i < 12; ++i) {
		 graph_circle(dotsHoursPos[i].x, dotsHoursPos[i].y, 3, c_black, true);
	}
	
	graph_circle(CX, CY, 5, c_black, true);
}


void myTimerHandler() {
	
	graph_line(CX, CY, secsPos[secs].x, secsPos[secs].y, c_white);
	secs++;
	if (secs == 60) {
		secs = 0;
		graph_line(CX, CY, minutsPos[minuts].x, minutsPos[minuts].y, c_white);
		minuts = (minuts + 1) % 60 ;
		if (minuts % 12 == 0) {
			graph_line(CX, CY, hoursPos[hours].x, hoursPos[hours].y, c_white);
			hours = (hours + 1) % 60;
		}
	 
	}	
	
	graph_line(CX, CY, hoursPos[hours].x, hoursPos[hours].y, c_black);
	
	graph_line(CX, CY, minutsPos[minuts].x, minutsPos[minuts].y, c_black);
		
	graph_line(CX, CY, secsPos[secs].x, secsPos[secs].y, c_blue);
	
	graph_circle(CX, CY, 5, c_black, true);
	
	printf("%02d:%02d:%02d\r",  hours, minuts, secs);
	fflush(stdout);
}
	




int main() {
	graph_init();
	
	hours = 3;
	minuts = 40;
	secs = 30;
	graph_regist_timer_handler(myTimerHandler, 1000);
	
	calculatePositions(secsPos, RADIUS-2, 60);
	calculatePositions(minutsPos, RADIUS-15, 60);
	calculatePositions(hoursPos, RADIUS-20, 12);
	
	drawInitialClock();
	
	graph_start();
	
	
	
}
	
		
