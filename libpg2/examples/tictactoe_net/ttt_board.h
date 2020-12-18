#define SIDE 3


#define BALL	1
#define CROSS	2
#define EMPTY 	0

// Play results


#define PLAY_INVALID   -1
#define PLAY_NO_WIN		0
#define PLAY_WIN		1
#define PLAY_DRAW		4


typedef struct {
	int board[SIDE][SIDE];
	int nplays;
} ttt_board_t;

int ttt_play(ttt_board_t *board, int x, int y, int piece);

void ttt_create_board(ttt_board_t *board);
