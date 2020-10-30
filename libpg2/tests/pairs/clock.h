
struct clock {
	int x, y;
	RGB color, back;
	int min, sec;
};
typedef struct clock Clock;

Clock create_clock(int x, int y, RGB color, RGB back);
void draw_clock(Clock c);
Clock tick(Clock c);
	 
