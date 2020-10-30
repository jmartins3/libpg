#define COUNTER_Y 350

#define COUNTER_DIGITS 4

#define CMARGIN_X 10
#define CMARGIN_Y 10




/*
 * Retorna uma estrutura size com as dimensões da fonte 
 * de texto especificada
 * Parâmetros:
 * 		font_size: dimensão da fonte (SMALL_FONT, MEDIUM_FONT, LARGE_FONT)
 * Retorna:
 * 		estrutura Size com as dimensões da fonte
 */
Size get_font_size(int font_size);

/*
 * Retorna uma estrutura size com as dimensões ocupado pelo texto "text"
 * na  dimensão de fonte font
 * Parâmetros:
 * 		text: string com o texto a avaliar
 * 		font: dimensão da fonte (SMALL_FONT, MEDIUM_FONT, LARGE_FONT)
 * Retorna:
 * 		estrutura Size com as dimensões do texto
 */
Size get_text_size(char text[], int font);

/*
 * Retorna uma estrutura size com as dimensões ocupado pelo número
 * de caracteres na  dimensão de fonte indicados
 * Parâmetros:
 * 		nchars: número de caracteres
 * 		font: dimensão da fonte (SMALL_FONT, MEDIUM_FONT, LARGE_FONT)
 * Retorna:
 * 		estrutura Size com as dimensões do texto
 */
Size get_chars_size(int nchars, int font);

// estrutura que representa um elemento ggráfico para afixar mensagens
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
	 

/*
 * Criação de elemento gráfico para afixar mensagens
 */
MsgView mv_create(int x, int y, int tchars, int mw, int mh, int fsize, RGB tc, RGB bc);
	 

/*
 * Apresentação de um dado numero através do MegView especificado
 */
void mv_show_number(MsgView nv, int num);

/*
 * Apresentação de uma dada string através do MsgView 
 * e alinhamento especificados
 */
void mv_show_text(MsgView mv, char  msg[], int align);

/*
 * Apresentação de uma dada string através do MsgView 
 * côr e alinhamento especificados
 */
void mv_show_clrtext(MsgView mv, char  msg[], RGB tcolor, int align);

/*
 * Estrutura e funções para criação e utilização de relógio digital 
 * com minutos e segundos
 */

// Representa um relógio com o MsgView para afixação
typedef struct {
	int m, s;		// minutos e segundos
	MsgView view;	// o MesgView usado para a afixação
} Clock;


#define SMALL_CLOCK SMALL_FONT
#define MEDIUM_CLOCK	MEDIUM_FONT
#define LARGE_CLOCK LARGE_FONT
/*
 *  Criação de relógio 
 *  Parâmetros:
 * 		x, y : coordenadas do canto superior esquerdo
 * 		tcolor: côr dos dígitos
 * 		bcolor: côr do fundo
 */
Clock clk_create(int x, int y, int size, RGB tcolor, RGB bcolor);
	 
/*
 *  Criação de relógio 
 *  Parâmetros:
 * 		x, y : coordenadas do canto superior esquerdo
 * 		tcolor: côr dos dígitos
 * 		bcolor: côr do fundo
 */
Clock clk_create_cron(int x, int y, int sm, int ss, RGB tcolor, RGB bcolor);


// mostrar tempo
void clk_show(Clock c);
	 

// incremento de um segundo
Clock clk_tick(Clock c);
	 
// decremento de um segundo
Clock clk_down_tick(Clock c);

// Reset
Clock clk_reset(Clock c);

/*
 * Estrutura e funções para lidar com contadores
 */

 
 // definição de estrutura e funções para representar um contador com título
typedef struct Counter {
	int val;
	MsgView view;
} Counter;


// criação de um contador na posição dada
Counter ct_create(int x, int y, int initial, char title[], int font_size);
	 
// apresentação do contador
void ct_show(Counter c);
	 
// retorna um contador que resulta do incremento do contador dado
Counter ct_inc(Counter c);

// retorna um contador que resulta do decremento do contador dado
Counter ct_dec(Counter c);
	 
/*
 * Estrutura e funções para lidar com botões
 */

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


// criação de botão com texto e posição dados
Button bt_create(int x, int y, char text[], RGB text_color);

// desenho de botão 
void bt_draw(Button b, int state);

// verifica se o ponto está contido no botão no caso do botão estar enabled
// caso o botão esteja disabled retorna sempre false;
bool bt_selected(Button b, int x, int y);
	
// modifica a cor do texto do botão
Button bt_set_text_color(Button bt, RGB text_color);

// coloca o botão enabled ou disabled
Button bt_set_enable(Button bt, bool enabled);

// verifica se o botão está enabled
bool bt_is_enabled(Button bt);



/*
 * Estrutura e funções para lidar com check boxes
 */


#define CBOX_MAX_TEXT 20
#define CBOX_BACK_COLOR c_white

typedef struct _checkbox {
	bool checked;
	// A completar
	
} CheckBox;


CheckBox cbox_create(int x, int y, char label[], 
					 int font_size, 
					 RGB text_color,
					 RGB check_color,
					 RGB back_color);
	


void cbox_draw(CheckBox cbox);
	 
void cbox_hide(CheckBox cbox);

bool cbox_is_checked(CheckBox cbox);
	 

bool cbox_selected(CheckBox cbox, int x, int y);

CheckBox cbox_toggle(CheckBox cbox);
