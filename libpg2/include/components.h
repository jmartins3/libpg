


 

// estrutura que representa um elemento gráfico para afixar mensagens
typedef struct {
	Point loc;			// posição do canto superior esquerdo da mensagem
	int max_chars;		// máximo de caracteres do texto
	Size margins;		// dimensões das margens da mensagem	
	int font_size;		// dimensão da fonte a utilizar
	RGB text_color;		// côr do texto
	RGB back_color; 	// côr do fundo		
} MsgView;

// Possibilidades de alinhamento do texto na mensagem

#define ALIGN_CENTER 1
#define ALIGN_RIGHT  2
#define ALIGN_LEFT   3


// Definições para lidar com botões


#define MAX_BUT_TEXT 32
#define BUTTON_MARGIN 15
#define RELEASED_BUT_CLR graph_rgb(105,105,105)
#define PRESSED_BUT_CLR graph_rgb(80,80,80)

typedef struct button {
	char text[MAX_BUT_TEXT];
	Rect area;
	RGB text_color;
	bool enabled;
} Button;


// definições para suporte a relógios


// Representa um relógio com o MsgView para afixação
typedef struct {
	int m, s;		// minutos e segundos
	MsgView view;	// o MesgView usado para a afixação
} Clock;


#define SMALL_CLOCK SMALL_FONT
#define MEDIUM_CLOCK	MEDIUM_FONT
#define LARGE_CLOCK LARGE_FONT


// definições para suporte a contadores
 
 // contador com título
typedef struct Counter {
	int val;
	MsgView view;
} Counter;


#ifdef __cplusplus 
extern "C" {
#endif

 

/*
 * Criação de elemento gráfico para afixar mensagens
 */
void mv_create(MsgView  *mv, int x, int y, int tchars, int fsize, RGB tc, RGB bc);
	 

/*
 * Apresentação de um dado numero através do MegView especificado
 */
void mv_show_number(MsgView* nv, int num);

/*
 * Apresentação de uma dada string através do MsgView 
 * e alinhamento especificados
 */
void mv_show_text(MsgView* mv, const char  msg[], int align);

/*
 * Apresentação de uma dada string através do MsgView 
 * côr e alinhamento especificados
 */
void mv_show_clrtext(MsgView* mv, const char  msg[], RGB tcolor, int align);


void mv_set_clr(MsgView *mv, RGB clr);
	 

void mv_set_margins(MsgView *mv, int mw, int mh);
	 


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
void clk_create(Clock  *c, int x, int y, int font, RGB tcolor, RGB bcolor);
	 
/*
 *  Criação de relógio 
 *  Parâmetros:
 * 		x, y : coordenadas do canto superior esquerdo
 * 		tcolor: côr dos dígitos
 * 		bcolor: côr do fundo
 */
void clk_create_cron(Clock *c, int x, int y, int sm, int ss, int font, RGB tcolor, RGB bcolor);


// mostrar tempo
void clk_show(Clock *c);
	 

// incremento de um segundo
void clk_tick(Clock *c);
	 
// decremento de um segundo
Clock clk_down_tick(Clock c);

// Reset
void clk_reset(Clock *c);



/*
 *Funções para lidar com contadores
 */



// criação de um contador na posição dada
void ct_create(Counter  *ct, int x, int y, int initial, char title[], int font_size);
	 
// apresentação do contador
void ct_show(Counter *c);
	 
// retorna um contador que resulta do incremento do contador dado
void ct_inc(Counter *c);

// retorna um contador que resulta do decremento do contador dado
void ct_dec(Counter *c);
	 
/*
 * Funções para lidar com botões
 */

 
// criação de botão com texto e posição dados
void bt_create(Button  *b, int x, int y, char text[], RGB text_color);

// desenho de botão 
void bt_draw(Button *bt, int state);

// verifica se o ponto está contido no botão no caso do botão estar enabled
// caso o botão esteja disabled retorna sempre false;
bool bt_selected(Button *bt, int x, int y);
	
// modifica a cor do texto do botão
void bt_set_text_color(Button *bt, RGB text_color);

// coloca o botão enabled ou disabled
void bt_set_enable(Button *bt, bool enabled);

// verifica se o botão está enabled
bool bt_is_enabled(Button *bt);


#ifdef __cplusplus 
}
#endif 
