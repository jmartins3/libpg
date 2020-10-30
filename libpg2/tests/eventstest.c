/*---------------------------------------------------------
 libgraph events tester 
 
 Mostra na consola a informação associada a eventos de rato, teclado e temporização
 
  gcc -o eventstest -std=c99 -Wall eventstest.c -lpg
   
 Jorge Martins, 2014
 ---------------------------------------------------------*/
 
 
#include <stdio.h>


#include "pg/graphics.h"
#include <ctype.h>


/* handlers de eventos*/
 
/* mostra informações associadas a um evento de teclado */	
void myKeyEventHandler(KeyEvent ke) {
	printf("KeyEvent! (%s, %d, %c)\n", 
			ke.state == KEY_PRESSED ? "Pressed" : "Released", ke.keysym, isprint(ke.keysym) ? ke.keysym : '.'); 	
 
}

/* mostra informações associadas a um evento de alteração do estado dos botões de rato */
void myMouseEventHandler(MouseEvent me) {
	if (me.type == MOUSE_BUTTON_EVENT) {
		printf("MouseEvent! (%d,%d,%s,%s)\n", 
			me.x, me.y, me.button == BUTTON_LEFT ? "Left": "Right", 
			me.state == BUTTON_CLICK ? "Click" : (me.state == BUTTON_PRESSED ? "Pressed" : "Released")); 	
	}
}


/* evento de timer */
void myTimerEventHandler() {
	printf("TimerEvent!\n");  
}



int main() {
	// intialização da biblioteca gráfica
	if (graph_init() == -1) {
		printf("Error initializing graph library!\n");
		return (1);
	}
	 
 	
	// regista a função que processa eventos de teclado
	graph_regist_key_handler(myKeyEventHandler);
	
	// regista a função que processa eventos de timer (segundo a segundo)
	graph_regist_timer_handler(myTimerEventHandler, 1000);
	
	// regista a função que processa eventos de rato  
	graph_regist_mouse_handler(myMouseEventHandler);
	
	
	// inicia o processamento de eventos e o refrescamento da janela
	graph_start();
	
	return 0;
}
