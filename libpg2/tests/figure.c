// ficheiro de include necessário
#include "pg/graphics.h"

int main() {
	// chamada necessária antes da iniciação da biblioteca
	graph_init();
	
	// desenho de um triângulo dados os pontos dos vértices
	// o penúltimo parâmetro indica a côr e o último que deve ser desenhado a cheio
	graph_triangle(150, 300, 450, 300, 300,  50,  c_green, true);
	
	// desenho de um segundo triângulo dados os pontos dos vértices
	graph_triangle(150, 150, 450, 150, 300,  400, c_green, true);
	
	// chamada necessária para correcto refrescamento da consola gráfica
	graph_start();
	return 0;
}
