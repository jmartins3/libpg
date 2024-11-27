#include <stdio.h>
#include <stdbool.h>

#include "pg/pglib.h"
#include "pairs.h"

#define GRAPH_WIDTH 1024
#define GRAPH_HEIGHT 768

// globals
char cardNames[][30] = {
	"c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "ct", "ca", "cj", "ck", "cq",
	"h2", "h3", "h4", "h5", "h6", "h7", "h8", "h9", "ht", "ha", "hj", "hk", "hq",
	"d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "dt", "da", "dj", "dk", "dq",
	"s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "st", "sa", "sj", "sk", "sq"
};


PairsGame game;
Clock clk;
Counter counter;
bool to_hide;
Location hide1, hide2;
int count_milis, ticks;
bool game_end;

Location remove1,remove2;

RemAnim ranim1, ranim2;




Card create_card(int value) {
	Card card;
	
	card.value = value;
	card.state = HIDDEN;
	return card;
}



RemAnim create_anim(int line, int col) {
	RemAnim ra;
	ra.line = line; ra.col = col;
	ra.width = CARD_WIDTH; ra.height = CARD_HEIGHT;
	ra.stopped=false;
	return ra;
}


int rand_between(int li, int ls) {
	return (rand() % (ls-li)) + li;
}


void show_vitory() {
	char msg[120];
	sprintf(msg, "You win in %d moves!", counter.val);
	graph_text(120, 230, graph_rgb(200,200,200), msg, LARGE_FONT);
}

void show_card_aux(int line, int col, int w, int h) {
	// obter coordenadas de ecrã
	int cx = BOARD_X + CARD_BORDER_X + (CARD_BORDER_X + CARD_WIDTH)*col;
	int cy = BOARD_Y - (CARD_BORDER_Y + CARD_HEIGHT)*(line+1);
	// obter carta
	Card card = game.cards[line][col];
	
	char cardPath[100];
	
	graph_rect(cx,cy, CARD_WIDTH, CARD_HEIGHT, BACK_COLOR, true);
	if (card.state == VISIBLE || card.state == REMOVED) {
		sprintf(cardPath, "cartas/%s.png", cardNames[card.value]);
		graph_image(cardPath, cx, cy, w, h);
	}
	else if (card.state == HIDDEN) {
		graph_image("cartas/b.png", cx, cy, w, h);
	}
}

RemAnim step_anim(RemAnim anim) {
	if (!anim.stopped) {
		show_card_aux(anim.line, anim.col, anim.width, anim.height);
		anim.width -= 4; anim.height -= 5;
		if (anim.width < 0 ) anim.width = 0;
		if (anim.height < 0) anim.height  = 0;
		if (anim.width == 0 && anim.height == 0) {
			game.remaining  -=1;
			if (game.remaining == 0) {
				show_vitory();
				game_end=true;
			}
			anim.stopped = true;
		}
	}
	return anim;
}

void show_card(int line, int col) {
	show_card_aux(line, col, CARD_WIDTH, CARD_HEIGHT);
}

void build_board() {
	 
	
	// define card values
	int values[NVALUES];

	int nvals = NVALUES;
	// initialize values
	for (int i=0; i < NVALUES; i+=2) {
		values[i] = rand_between(0,52);
		values[i+1] = values[i];
		
	}
	
	// build the board
	for (int l=0; l < NLINES; ++l) {
		for(int c=0; c < NCOLS; ++c) {
			int index = rand_between(0, nvals);
			game.cards[l][c] = create_card(values[index]);
			values[index] = values[--nvals];
		}
	}
	
	game.first.line =-1;
	game.remaining = NVALUES;
	clk_create(&clk, 10,30, MEDIUM_CLOCK,  c_orange, BACK_COLOR);
	ct_create(&counter, 560,30, 0, "Jogadas: ", MEDIUM_FONT);
	
	
}


Location graph_to_board_loc(int x, int y) {
	Location l;
	
	if (x < (BOARD_X + CARD_BORDER_X) || x > BOARD_X + BOARD_WIDTH || y < BOARD_Y - BOARD_HEIGHT
	  || y > BOARD_Y - CARD_BORDER_Y) {
		  l.line = l.col = -1;
	}
	else {
		l.col = (x- BOARD_X)/(CARD_BORDER_X+CARD_WIDTH);
		l.line = (BOARD_Y - y) /(CARD_BORDER_Y+CARD_HEIGHT);
	 
	}
	return l;
}

void draw_board() {
	graph_rect(0,0,GRAPH_WIDTH, GRAPH_HEIGHT, BACK_COLOR, true);
	for (int l=0; l < NLINES; ++l) {
		for(int c=0; c < NCOLS; ++c) {
			 show_card(l,c);
		}
	}
	ct_show(&counter);
	clk_show(&clk);
}

int get_value(Location l) {
	return game.cards[l.line][l.col].value;
}

void hide_card(Location l) {
	game.cards[l.line][l.col].state = HIDDEN;
	show_card(l.line, l.col);
}



RemAnim remove_card(Location l) {
	game.cards[l.line][l.col].state = REMOVED;

	return create_anim(l.line, l.col);
}


void hide_pair(Location l1, Location l2) {
	hide1 = l1; hide2=l2;
	to_hide = true;
	count_milis=0;
}

void play(Location l) {
	if (game.cards[l.line][l.col].state != HIDDEN || to_hide || !ranim1.stopped ||
	 !ranim2.stopped) return;
	ct_inc(&counter);
	ct_show(&counter);

	game.cards[l.line][l.col].state = VISIBLE;
	if (game.first.line == -1) {
		game.first = l;
	}
	else {
		if (get_value(game.first) == get_value(l)) {
			
			ranim1 = remove_card(game.first );
			ranim2 = remove_card(l );
			
		}
		else {
			hide_pair(game.first, l );
		}	
		game.first.line = -1;
	}	count_milis += BASE_TIME;
	show_card(l.line, l.col);
}

void mouseHandler(MouseEvent me) {
	if (me.type != MOUSE_BUTTON_EVENT || me.state != BUTTON_PRESSED)
		return;
	Location l = graph_to_board_loc(me.x, me.y);
	if (l.line != -1)
		play(l);
}

void timerHandler() {
	if (game_end) return;
	if (to_hide) {
		count_milis += BASE_TIME;
		if (count_milis >= HIDE_TIME) {
			hide_card(hide1);
			hide_card(hide2);
			to_hide = false;
			
		}
	}
	if (!ranim1.stopped) 
		ranim1 = step_anim(ranim1);
	if (!ranim2.stopped) 
		ranim2 = step_anim(ranim2);
	++ticks;
	if (ticks == 20) {
		clk_tick(&clk);
		clk_show(&clk);
		ticks=0;
	}
}


void keyHandler(KeyEvent ke) {
	if (ke.state == KEY_PRESSED) {
		if (game.remaining == 0) graph_exit();
	 
		else {
			if (ke.keysym >= '1' && ke.keysym <= '9') {
				char name[100];
				sprintf(name, "screen%c.jpg", ke.keysym);
				graph_save_screen(name);
			}
		}
		 
	}
}

int main() {
	graph_init();
 
	
	ranim1.stopped=true;
	ranim2.stopped=true;
	build_board();
	draw_board();
	graph_regist_mouse_handler(mouseHandler);
	graph_regist_key_handler(keyHandler);
	graph_regist_timer_handler(timerHandler, BASE_TIME);

	graph_start();
	return 0;
}
