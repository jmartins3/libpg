 

#define GRAPH_WIDTH 1024
#define GRAPH_HEIGHT 768

/* constant definitions */

#define NLINES	6
#define NCOLS	7


typedef struct Hole {
	int color;
	Point center;
	int	x, y;
} Hole;

typedef struct FourBoard {
	Hole  holes[NLINES][NCOLS];
	int height[NCOLS];
} FourBoard;



/* players colours */

#define YELLOW	1
#define RED		2



#define BOARD_CORNER_X	50
#define BOARD_CORNER_Y	20

#define BOARD_WIDTH (GRAPH_WIDTH - 2*BOARD_CORNER_X)
#define BOARD_HEIGHT (GRAPH_HEIGHT - 2*BOARD_CORNER_Y)

#define HOLE_BORDER		10

#define HOLE_WIDTH 		(BOARD_WIDTH / NCOLS)
#define HOLE_RADIUS 	(HOLE_HEIGHT/2 - 2*HOLE_BORDER)
#define HOLE_HEIGHT (BOARD_HEIGHT / NLINES)


/* players colours */

#define YELLOW	1
#define RED		2



#define BOARD_CORNER_X	50
#define BOARD_CORNER_Y	20

#define BOARD_WIDTH (GRAPH_WIDTH - 2*BOARD_CORNER_X)
#define BOARD_HEIGHT (GRAPH_HEIGHT - 2*BOARD_CORNER_Y)

#define HOLE_BORDER		10

#define HOLE_WIDTH 		(BOARD_WIDTH / NCOLS)
#define HOLE_RADIUS 	(HOLE_HEIGHT/2 - 2*HOLE_BORDER)
#define HOLE_HEIGHT (BOARD_HEIGHT / NLINES)


// functions

bool is_draw(FourBoard *board);
bool winner(FourBoard *board, int player) ;


void PointInit(Point *p, int x, int y);
	 

void HoleInit(Hole *h, Point p, int x, int y);
