#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include "pg/graphics.h"
#include  "utils.h"

// iniciar o gerador de números pseudo-aleatórios
void irandom_init() {	
	srand(time(NULL));
}

// obter um número aleatório dentro do intervalo (fechado) especificado
int irandom(int li, int ls) {
	return rand() % (ls -li +1) + li;
}


/*
 * Point
 */
 
/*
 *  Calcular a distância entre dois pontos
 */
double point_distance(Point p1, Point p2) {
	int dx = p1.x - p2.x;
	int dy = p1.y - p2.y;
	
	return sqrt( dx*dx + dy*dy);
}

/* 
 * Verificar se um ponto está contido no rectângulo dado
 */
bool rect_contains(Rect r, Point p) {
	return p.x >= r.p.x && p.x < r.p.x + r.sz.width &&
		   p.y >= r.p.y && p.y < r.p.y + r.sz.height;
}
