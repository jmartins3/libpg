/*-----------------------------------------------------------------------
 *  Utiliza a console gráfica fornecida pela biblioteca de funções de pg (pglib)
 * para desenhar triângulos dados os seus pontos ou os seus lados e um dos pontos
 * 
 * Notas:
 * 	Se ainda não o tiver feito terá de instalar a última versão da
 *  biblioteca fornecida como recurso do toth.
 * 
 *  Após a instalação terá de usar o switch -lpg no final do comando de compilação
 *  para incluir no programa as funções usadas.
 * 
 *  Jorge Martins, 2014
 *-------------------------------------------------------------------*/

#define _XOPEN_SOURCE 500
#define GRAPHICS_IMPL

#include "../include/graphics.h"
#include <math.h>


#define min(x,y) ((x) < (y) ? (x) : (y))
 
/*
 *  determina se os três valores a, b e c
 *  podem corresponder aos lados de um triângulo
 */
static int isTriangle(int a, int b, int c) {
	return a < b + c && b < a + c && c < a + b;
}




/*
 * faz fill do triângulo cujos pontos estão indicados
 * Assume-se que a base do triângulo está na horizontal (quer dizer que há dois pontos com
 * ordenada idêntica)
 */
static void graph_triangle_fill_aux(int x0, int y0, int x1, int y1, int x2, int y2, RGB color) {
	int tx, ty, by, xb0, xb1;
	
	if (y0 == y1)  { // base é (x0,y0), (x1,y1)
		tx = x2; ty = y2; by = y0; xb0 = x0; xb1 = x1;
	}
	else if (y0 == y2) { // base é (x0,y0), (x2,y2)
		tx = x1; ty = y1; by = y0; xb0 = x0; xb1 = x2;
	}
	else if (y1 == y2) { // base é (x1,y1), (x2,y2)
		tx = x0; ty = y0; by = y1; xb0 = x1; xb1 = x2;
	}
	else { // a base não está na horizontal
		return;
	}
	
		
	// agora o fill
	// determinar a equacao das rectas do topo ao os outros vertices
	//
	//  (y- y1)/(x - x1) = (y1 - y0)/ (x1 - x0)
	
	double m0 = (double) (xb0 - tx) / (by - ty);
	double m1 = (double) (xb1 - tx) / (by - ty);
	
	if (ty < by) {
		for (int y = ty; y <= min(graph_get_height()-1,by); ++y) {
			
			int x1 = round((y - by) * m0 + xb0);
			int x2 = round((y - by) * m1 + xb1);
			// draw horizontal line between x1 and x2)
			graph_line(x1, y, x2, y, color);
		}
	}
	else {
		for (int y = min(graph_get_height()-1,ty); y >= by; --y) {
			
			int x1 = round((y - by) * m0 + xb0);
			int x2 = round((y - by) * m1 + xb1);
			// draw horizontal line between x1 and x2)
			graph_line(x1, y, x2, y, color);
		}
	}
	graph_refresh();
}


static void graph_triangle_fill(int x0, int y0, int x1, int y1, int x2, int y2, RGB color) {
	// sort points by y  (x0, y0), (x1, y1), (x2 y2) in that (growing) order
	int aux;
	
	if (y0 > y1) {
		aux = x0; x0 = x1; x1 = aux;
		aux = y0; y0 = y1; y1 = aux;
	}
	if (y0 > y2) {
		aux = x0; x0 = x2; x2 = aux;
		aux = y0; y0 = y2; y2 = aux;
	}
	if (y1 > y2) {
		aux = x1; x1 = x2; x2 = aux;
		aux = y1; y1 = y2; y2 = aux;
	}
	// now fill the partial triangle 
	// determined by the horizontal line starting at x1 and intersecting the segment
	// (x0, y0), (x2, y2)
	double m = (double) (x2 - x0) / (y2 - y0);
	int x = round((y1 - y2) * m + x2);
	
	
	if (y0 < y1)
		graph_triangle_fill_aux(x1, y1, x, y1, x0, y0, color);	
	if (y1 < y2)
		graph_triangle_fill_aux(x1, y1, x, y1, x2, y2, color);	
		
}
	
/*
 * desenho de um triângulo dados os seus pontos
 */
void graph_triangle(int x0, int y0, int x1, int y1, int x2, int y2, RGB color, bool toFill ) {
	
	if (toFill) 
		graph_triangle_fill(x0, y0, x1, y1, x2, y2, color); 
	else {
		graph_line(x0, y0, x1, y1, color);
		graph_line(x1, y1, x2, y2, color);
		graph_line(x2, y2, x0, y0, color);
	}
 
	// a chamada seguinte força a actualização da janela
	graph_refresh();
}

int graph_triangle2(int xb0, int yb0, int a, int b, int c, RGB color, bool toFill) {
	// se não for triângulo a função retorna o valor falso(0)
	if (!isTriangle(a, b, c)) 
		return 0;
	
	// calcula o segundo ponto (xb1, yb1)  assumindo qua a base (b) está na horizontal
	int xb1 = xb0 + b, yb1 = yb0;
	
	// utiliza o teorema dos senos para determinar o terceiro ponto (xa0, ya0)
	// do triângulo a partir dos seus lados
	double aux = (double)(a*a + b*b - c*c) /  (2*b);
	
	int h = (int) round(sqrt(a*a - aux * aux));
	
	int ya0 = yb0 - h;
	
	// teorema de pitágoras
	int xa0 = (int)  round(xb0 + sqrt( a*a - h* h));
	
	
	// desenho do triângulo com a cor indicada
	graph_triangle(xb0, yb0, xb1, yb1, xa0, ya0, color, toFill); 
	
	//graph_triangle_fill(xb0, yb0, xb1, yb1, xa0, ya0, color); 
	// o retorno de "1" (true)  indica  sucesso na execução da função
	return 1;
}
