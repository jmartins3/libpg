#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <pg/graphics.h>
#include <stdbool.h>


#include "components.h"


#define MAX_DIGITS 32

#define MV_DEF_MARGIN_W  5
#define MV_DEF_MARGIN_H  5
 

 
/*
 * Criação de elemento gráfico para afixar mensagens
 */
MsgView mv_create(int x, int y, int tchars,  int font, RGB tc, RGB bc) {
	
	MsgView mv = { 
		{x, y}, // coordenadas do canto superior esquerdo
		tchars, // máximo de caracteres da mensagem
		{MV_DEF_MARGIN_W, MV_DEF_MARGIN_H}, // dimensões das margens de visualização
		font, 	// dimensão da fonte
		tc, 	// côr do texto
		bc 		// côr do fundo
	};
	return mv;
}

void mv_set_clr(MsgView *mv, RGB clr) {
	mv->text_color = clr;
}

void mv_set_margins(MsgView *mv, int mw, int mh) {
	mv->margins.width = mw;
	mv->margins.height = mh;
}


/*
 * Apresentação de uma dada string através do MsgView 
 * e alinhamento especificados
 */
void mv_show_clrtext(MsgView * mv, char  msg[], RGB tcolor, int align) {
	 
	Size s = graph_text_size(msg, mv->font_size);
	
	Size sm = graph_chars_size(mv->max_chars, mv->font_size);
	graph_rect(mv->loc.x, mv->loc.y, 
				sm.width + 2*mv->margins.width, 
			    sm.height + 2*mv->margins.height, 
			    mv->back_color, true);
	int tx;
	if (align == ALIGN_CENTER)
		tx = mv->loc.x + mv->margins.width + (sm.width - s.width)/2;
	else if (align == ALIGN_RIGHT)
		tx = mv->loc.x + mv->margins.width + sm.width - s.width;
	else
		tx = mv->loc.x + mv->margins.width;
	int ty = mv->loc.y + mv->margins.height + s.height;

	//graph_rect(mv.loc.x + mv.margins.width + (sm.width - s.width)/2, mv.loc.y + mv.margins.height, s.width, s.height, graph_rgb(240,240,255), true);
	graph_text(tx, ty, tcolor, msg, mv->font_size);
}

/*
 * Apresentação de uma dada string através do MsgView 
 * e alinhamento especificados
 */
void mv_show_text(MsgView *mv, char  msg[], int align) {
	 mv_show_clrtext(mv, msg, mv->text_color, align);
	 
}


/*
 * Apresentação de um dado numero através do MsgView especificado
 */
void mv_show_number(MsgView* mv, int num) {
	char snum[MAX_DIGITS];
	sprintf(snum, "%d", num);
	mv_show_text(mv, snum, ALIGN_RIGHT);
}

/*
 * Funções para criação e utilização de relógio digital 
 * com minutos e segundos
 */

/*
 *  Criação de relógio 
 *  Parâmetros:
 * 		x, y : coordenadas do canto superior esquerdo
 * 		tcolor: côr dos dígitos
 * 		bcolor: côr do fundo
 */
Clock clk_create(int x, int y, int size, RGB tcolor, RGB bcolor) {
	Clock c;
	c.view = mv_create(x, y, 5, size, tcolor, bcolor);
	mv_set_margins(&c.view, 20, 10);
	c.m = c.s = 0;
	return c;
}

Clock clk_create_cron(int x, int y, int sm, int ss, RGB tcolor, RGB bcolor) {
	Clock c;
	c.view = mv_create(x, y, 5,  LARGE_FONT, tcolor, bcolor);
	mv_set_margins(&c.view, 20, 10);
	c.m = sm; c.s = ss;
	return c;
}

// mostrar tempo
void clk_show(Clock c) {
	char text[20];
	sprintf(text, "%02d:%02d", c.m, c.s);
	mv_show_text(&c.view, text, ALIGN_CENTER);
}

// incremento de um segundo
Clock clk_tick(Clock c) {
	c.s++;
	if (c.s >= 60) {
		c.s = 0;
		c.m++;
		if (c.m >= 60) 
			c.m = 0;
	}
	return c;	
}

Clock clk_down_tick(Clock c) {
	if (c.s == 0 && c.m == 0) return c;
	c.s--;
	if (c.s < 0) {
		c.s = 59;
		c.m--;
	}
	return c;	
}

Clock clk_reset(Clock c) {
	c.m = 0;
	c.s = 0;
	return c;
	
}

// funções para representar um contador com título
 

// criação de um contador na posição dada
Counter ct_create(int x, int y, int initial, char title[], int font_size) {
	Counter c;
	Size nameSize = graph_text_size(title, MEDIUM_FONT);
	// mostrar nome
	graph_text(x, y+ nameSize.height+CMARGIN_Y, c_orange, title, MEDIUM_FONT);
	c.val=initial;
	c.view = mv_create(x + nameSize.width+10, y, 
						COUNTER_DIGITS, 
					    font_size, c_black, c_gray);
	mv_set_margins(&c.view, CMARGIN_X, CMARGIN_Y);
	return c;
}

// apresentação do contador
void ct_show(Counter c) {
	mv_show_number(&c.view, c.val);
}

// retorna um contador que resulta do incremento do contador dado
Counter ct_inc(Counter c) {
	c.val++;
	ct_show(c);
	return c;
}

// retorna um contador que resulta do incremento do contador dado
Counter ct_dec(Counter c) {
	if (c.val>0) c.val--;
	ct_show(c);
	return c;
}


/*
 * Funções para lidar com botões
 */
 


// criação de um botão com o texto e posição dada
Button bt_create(int x, int y, char text[], RGB text_color) {
	Button b;
	Size tsize = graph_text_size(text, MEDIUM_FONT);
	
	Rect area = { 
		.p = { x, y }, 
		.sz = { 
			tsize.width + 2*BUTTON_MARGIN, tsize.height + 2*BUTTON_MARGIN
		}
	};
	
	b.area = area;
	strncpy(b.text, text, MAX_BUT_TEXT);
	b.text[MAX_BUT_TEXT-1] =0;
	b.text_color = text_color;
	b.enabled = true;
	return b;
}

 
	
// desenha o botão consoante está premido ou largado
void bt_draw(Button b, int state) {
	Point p = b.area.p;
	Size sz = b.area.sz;
	 
	if (state == BUTTON_PRESSED)  {
		graph_rect(p.x, p.y, sz.width, sz.height, PRESSED_BUT_CLR, true);
		graph_text(p.x+ BUTTON_MARGIN , 
				   p.y + BUTTON_MARGIN+ graph_font_size(MEDIUM_FONT).height, 
				   b.text_color, b.text, MEDIUM_FONT);
		graph_rect(p.x, p.y, sz.width, sz.height, graph_rgb(40,40,40), false);
	}	 
	else {
		graph_rect(p.x, p.y, sz.width, sz.height, RELEASED_BUT_CLR, true);
		graph_text(p.x+ BUTTON_MARGIN, 
					p.y + BUTTON_MARGIN + graph_font_size(MEDIUM_FONT).height, 
					b.text_color, b.text, MEDIUM_FONT);
		graph_rect(p.x, p.y, sz.width, sz.height, graph_rgb(240,240,240), false);
	}
}

// verifica se o ponto está contido no botão no caso do botão estar enabled
// caso o botão esteja disabled retorna sempre false;
bool bt_selected(Button b, int x, int y) {
	if (b.enabled == false) return false;
	Point loc = b.area.p;
	Size sz = b.area.sz;
	return x >= loc.x && x < loc.x + sz.width &&
		   y >= loc.y && y < loc.y + sz.height;
}


// modica a cor do texto do botão
Button bt_set_text_color(Button bt, RGB text_color) {
	bt.text_color = text_color;
	return bt;
}

// coloca o botão enabled ou disabled
Button bt_set_enable(Button bt, bool enabled) {
	bt.enabled = enabled;
	return bt;
}

// verifica se o botão está enabled
bool bt_is_enabled(Button bt) {
	return bt.enabled;
}



 
