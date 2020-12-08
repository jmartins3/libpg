
 

#define CARD_WIDTH		110 	// 73
#define CARD_HEIGHT 	145		// 97
#define CARD_BORDER_X	15
#define CARD_BORDER_Y	8

#define NLINES			4
#define NCOLS			6

#define BOARD_X			66
#define BOARD_Y			700
#define BOARD_WIDTH    (CARD_BORDER_X + (CARD_BORDER_X + CARD_WIDTH)*NCOLS)
#define BOARD_HEIGHT   (CARD_BORDER_Y + (CARD_BORDER_Y + CARD_HEIGHT)*NLINES)

#define NVALUES (NLINES*NCOLS)
#define NPAIRS (NVALUES/2)

#define BASE_TIME 50
#define HIDE_TIME 2000

#define c_brown  graph_rgb(148, 79, 58)
#define c_dark_brown  graph_rgb(75, 40, 30) 
#define BACK_COLOR graph_rgb(35, 35, 35) 

#define VISIBLE 1
#define HIDDEN  2
#define REMOVED 3

struct location {
	int line;
	int col;
};
typedef struct location Location;


struct card {
	int value;
	int state;
};
typedef struct card Card;

struct pairs_game {
	Card cards[NLINES][NCOLS];
	int remaining;
	Location first;
};
typedef struct pairs_game PairsGame;

struct rem_anim {
	int width, height;
	int line, col;
	bool stopped;
};
typedef struct rem_anim RemAnim;

