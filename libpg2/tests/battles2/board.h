#pragma once

#define SQUARE_SIDE 20

#define FONT_SIZE MEDIUM_FONT

#define NSQUARES 10

// square vals
#define EMPTY 		0
#define SUBMARINE 	1	
#define CANNON_2	2
#define CANNON_3	3
#define CANNON_4	4
#define AIR_CARRIER	5

// colours
#define CLR_SUBMARINE 	c_yellow
#define CLR_CANNON_2  	c_dgray
#define CLR_CANNON_3  	c_blue
#define CLR_CANNON_4	c_orange
#define CLR_AIR_CARRIER	c_gray
#define CLR_WATER		graph_rgb(220,255,255)
#define CLR_TRY			graph_rgb(240,240,100)


// orientations
#define HORIZONTAL  0
#define VERTICAL 	1

#define BOAT_TYPES 	5
#define TOTAL_BOATS	4


#define TOTAL_PARTS (4*SUBMARINE+3*CANNON_2+2*CANNON_3+CANNON_4+AIR_CARRIER)

typedef struct opponent_info {
	Point shots[BOAT_TYPES][TOTAL_BOATS];
	int damaged[BOAT_TYPES][TOTAL_BOATS];
} opponent_info_t;

typedef struct board {
	int x, y;
	int places[NSQUARES][NSQUARES];
} board_t;

typedef struct battleship {
	board_t my_board, oppon_board;
	opponent_info_t op_stats;
	Point injuried;
	int explode_count;
	int last_target;
	Point last_play;
	int total_hits, total_injuries;
	int total_parts;
	bool result_send;
} battleship_t;

int rand_range(int ll, int hl);


void fill_place(board_t *board, int x, int y, int val);

void explode_place(board_t *board, int x, int y);

int val_place(board_t *board, int x, int y);

void fill_horiz_line(board_t *board, int rx, int ry, int sz, int val);

void fill_vert_line(board_t *board, int rx, int ry, int sz, int val);
	

void fill_line(board_t *board, int rx, int ry, int sz, int val, int orientation);

bool has_collisions(board_t *board, int rx, int ry, int size, int orientation);
	

int get_rand_orientation();

Point get_random_place(int horizPlaces, int vertPlaces);
	
void add_air_carrier(board_t *board);

bool  add_n_cannon_boat(board_t *board, int size);


void show_board(board_t *board);



void populate_battleship(battleship_t *battle);

void create_board(board_t *board, int x, int y);

void draw_place(board_t *board, int x, int y);

void shot_place(board_t *board, int x, int y);

void draw_place_try(board_t *board, int x, int y);

void draw_board(board_t *board, bool draw_places);

Point place_selected(board_t *board, int x, int y);

void hide_board(board_t *board) ;

void create_battleship(battleship_t *battle, int my_x, int my_y,
						int oppon_x, int oppon_y);