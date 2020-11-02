#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include "pg/graphics.h"
#include "board.h"


int rand_range(int ll, int hl) {
	int range = hl - ll + 1;
	return rand() % range + ll;
}

void fill_place(board_t *board, int x, int y, int val) {
	board->places[y][x] = val;
}

int val_place(board_t *board, int x, int y) {
	return board->places[y][x];
}

void fill_horiz_line(board_t *board, int rx, int ry, int sz, int val) {
	for(int x = rx; x < rx + sz; ++x)
		fill_place(board, x, ry, val);
}

void fill_vert_line(board_t *board, int rx, int ry, int sz, int val) {
	for(int y = ry; y < ry + sz; ++y) 
		fill_place(board, rx, y, val);
}

void fill_line(board_t *board, int rx, int ry, int sz, int val, int orientation) {
	if (orientation == VERTICAL)
		fill_vert_line(board, rx, ry, sz, val);
	else
		fill_horiz_line(board, rx, ry, sz, val);
}


bool has_collisions(board_t *board, int rx, int ry, int size, int orientation) {
	int xs, xe, ys, ye;
	
	if (orientation == HORIZONTAL) {
		xs = rx > 0 ? rx-1:rx;
		xe = rx < NSQUARES-1 ? rx+size: rx+size-1;
		
		ys = ry > 0 ? ry-1: ry;
		ye = ry < NSQUARES-1? ry + 1 : ry;
	}
	else {
		xs = rx > 0 ? rx-1:rx;
		xe = rx < NSQUARES-1 ? rx+1: rx;
		
		ys = ry > 0 ? ry-1: ry;
		ye = ry < NSQUARES-1? ry + size :ry + size-1;
	}
	
	for(int y = ys; y <= ye; ++y) {
		for(int x= xs; x <= xe; x++) {
			if (val_place(board, x , y) != EMPTY) return true;
		}
	}
		 
	 
	return false;
}

int get_rand_orientation() {
	return rand_range(0,1);
}

Point get_random_place(int horizPlaces, int vertPlaces) {
	Point p = { .x = rand_range(0, horizPlaces-1),
				.y = rand_range(0, vertPlaces-1) 
		};
	return p;
}
	
void add_air_carrier(board_t *board) {
	// put air_carrier
	
	int orientation =  get_rand_orientation(); // 0 - horiz, 1 - vertical
	int orient_other = (orientation +1) %2;
	Point origin = get_random_place(NSQUARES - 1 - orient_other*2,
									NSQUARES-1 - orientation*2);
	int lr_x = origin.x;
	int lr_y = origin.y;
	
	printf("orientation? %s\n", orientation ? "vertical": "horiz");
	printf("rx=%d,ry=%d\n", lr_x, lr_y);
	if (orientation == 1) {
		fill_vert_line(board, lr_x, lr_y, 3, AIR_CARRIER);
		if (lr_x < 2) lr_x++;
		else if (lr_x > NSQUARES - 3) lr_x -= 2;
		else if (rand_range(0,1) == 0) lr_x -= 2;
		else lr_x++;
		fill_horiz_line(board, lr_x, lr_y + 1, 2, AIR_CARRIER);
	}
	else {
		fill_horiz_line(board, lr_x, lr_y, 3, AIR_CARRIER);
		if (lr_y < 2) lr_y++;
		else if (lr_y > NSQUARES - 3) lr_y -= 2;
		else if (rand_range(0,1) == 0) lr_y -= 2;
		else lr_y++;
		fill_vert_line(board, lr_x+1, lr_y, 2, AIR_CARRIER);
	}	
}

void add_n_cannon_boat(board_t *board, int size) {
	int orientation, rx, ry;
	do {
		orientation = get_rand_orientation();
		int orient_other = (size == 1) ? orientation : (orientation +1) %2;
		Point origin = get_random_place(NSQUARES - 1 - orient_other*(size-2),
									NSQUARES-1 - orientation*(size-2));
		rx = origin.x;
		ry = origin.y;	 
	}
	while (has_collisions(board, rx, ry, size, orientation));
	printf("fill %d cannon boat\n", size);
	fill_line(board, rx, ry, size, size, orientation);
}

void show_board(board_t *board) {
	for(int i=0; i < NSQUARES; ++i) {
		for(int j =0; j < NSQUARES; ++j) {
			printf("%d ", val_place(board, j, i));
		}
		printf("\n");
	}
}

void board_clear(board_t *board) {
	for(int i=0; i < NSQUARES; ++i) {
		for(int j =0; j < NSQUARES; ++j) {
			fill_place(board, j, i, EMPTY);
		}
		printf("\n");
	}
}

void populate_board(board_t *board) {
	board_clear(board);

	add_air_carrier(board);
	
	// put four cannon
	add_n_cannon_boat(board, CANNON_4);
	
	// put two three cannon boats
	add_n_cannon_boat(board, CANNON_3);
	add_n_cannon_boat(board, CANNON_3);
	
	// put three two cannon boats
	add_n_cannon_boat(board, CANNON_2);
	add_n_cannon_boat(board, CANNON_2);
	add_n_cannon_boat(board, CANNON_2);
	
	// put four one cannon boats
	add_n_cannon_boat(board, SUBMARINE);
	add_n_cannon_boat(board, SUBMARINE);
	add_n_cannon_boat(board, SUBMARINE);
	add_n_cannon_boat(board, SUBMARINE);
	
}

void create_board(board_t *board, int x, int y) {
	board->x = x;
	board->y = y;
	for(int i=0; i < NSQUARES; ++i) {
		for(int j =0; j < NSQUARES; ++j) {
			board->places[i][j] = 0;
		}
	}
		
}

void create_battleship(battleship_t *battle, int my_x, int my_y,
						int oppon_x, int oppon_y) {
	create_board(&battle->my_board, my_x, my_y);
	create_board(&battle->oppon_board, oppon_x, oppon_y);
	battle->total_hits = 0;
	battle->total_injuries = 0;
	battle->last_target = -1;
	battle->total_parts = 0;
}


void populate_battleship(battleship_t *battle) {
	populate_board(&battle->my_board);
	battle->total_parts = TOTAL_PARTS;
}


void draw_place(board_t *board, int x, int y) {
	int screen_x = board->x + x*SQUARE_SIDE + 2;
	int screen_y = board->y + y*SQUARE_SIDE + 2;
	switch(val_place(board, x, y)) {
		case AIR_CARRIER:
			graph_rect(screen_x, screen_y, SQUARE_SIDE-3, SQUARE_SIDE-3, CLR_AIR_CARRIER, true);
			break;
		case CANNON_4:
			graph_rect(screen_x, screen_y, SQUARE_SIDE-3, SQUARE_SIDE-3, CLR_CANNON_4, true);
			break;
		case CANNON_3:
			graph_rect(screen_x, screen_y, SQUARE_SIDE-3, SQUARE_SIDE-3, CLR_CANNON_3, true);
			break;
		case CANNON_2:
			graph_rect(screen_x, screen_y, SQUARE_SIDE-3, SQUARE_SIDE-3, CLR_CANNON_2, true);
			break;
		case SUBMARINE:
			graph_rect(screen_x, screen_y, SQUARE_SIDE-3, SQUARE_SIDE-3, CLR_SUBMARINE, true);
			break;
		default:
			graph_rect(screen_x, screen_y, SQUARE_SIDE-3, SQUARE_SIDE-3, CLR_WATER, true);
			break;
	}
	graph_refresh();
}

void draw_place_try(board_t *board, int x, int y) {
	int screen_x = board->x + x*SQUARE_SIDE + 2;
	int screen_y = board->y + y*SQUARE_SIDE + 2;
	
	graph_rect(screen_x, screen_y, SQUARE_SIDE-3, SQUARE_SIDE-3, CLR_TRY, false);
	graph_rect(screen_x+1, screen_y+1, SQUARE_SIDE-5, SQUARE_SIDE-5, CLR_TRY, false);
}



static void hide_place(board_t *board, int x, int y) {
	int screen_x = board->x + x*SQUARE_SIDE + 2;
	int screen_y = board->y + y*SQUARE_SIDE + 2;
	 
	graph_rect(screen_x, screen_y, SQUARE_SIDE-3, SQUARE_SIDE-3, c_white, true);
}

void draw_board(board_t *board, bool draw_places) {
	// draw grid
	// horizontal
	int x = board->x;
	int y = board->y;
	int size = NSQUARES*SQUARE_SIDE;
	// draw letters
	
	char letter[2] = {0};
	for(int i=0; i <  NSQUARES; ++i) {
		letter[0] = 'A' +i;
		graph_text(x + i*SQUARE_SIDE + SQUARE_SIDE/3, y-2, c_dgray,  letter,FONT_SIZE);
		
	}
	
	// draw numbers
	
	char number[3] = {' ', 0};
	for(int i=0; i <  NSQUARES; ++i) {
		if ((i+1) /10 == 1) number[0] = '1';
		number[1] = '0' + (i+1)%10;
		graph_text(x -25, y + (i+1)*SQUARE_SIDE-2, c_dgray,  number, FONT_SIZE);
		
	}
	
	for(int i=0; i <= NSQUARES; ++i) {
		graph_line(x, y, x+ size, y, c_black);
		y+= SQUARE_SIDE;
	}
	// vertical
	y = board->y;
	for(int i=0; i <= NSQUARES; ++i) {
		graph_line(x, y, x, y+size, c_black);
		x+= SQUARE_SIDE;
	}
	
	graph_pixel(board->x+size, y+size+1, c_black);
	
	if (draw_places) {
		for(int i=0; i < NSQUARES; ++i) {
			for(int j = 0; j < NSQUARES; ++j) {
				draw_place(board,j, i);
			}
		}
	}
}


void hide_board(board_t *board) {
	 
	 
	for(int i=0; i < NSQUARES; ++i) {
		for(int j = 0; j < NSQUARES; ++j) {
			hide_place(board,j, i);
		}
	}
}

int shot_place(board_t *board, int x, int y) {
	if (x < 0 || x >= NSQUARES || y < 0 || y > NSQUARES) return -1;
	return val_place(board, x, y);
}

Point place_selected(board_t *board, int x, int y) {
	int size = NSQUARES * SQUARE_SIDE;
	if (x < board->x || y < board->y || 
	    x > board->x + size || y > board->y + size) {
			Point p = {-1,-1};
			return p;
	}
	
	Point p = { (x - board->x ) / SQUARE_SIDE,  (y - board->y ) / SQUARE_SIDE };
	return p;
}

