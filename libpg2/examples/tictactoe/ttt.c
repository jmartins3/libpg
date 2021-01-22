/*------------------------------------------
 O jogo do galo
 
 Jorge Martins, 2020
 
 -----------------------------------------*/
 
 
#include <stdio.h>
#include <stdbool.h>
#include <math.h>


#include "pg/pglib.h"

#include "ttt.h" 
 

// variáveis globais 

bool game_over;
ttt_board_t theBoard;
int turn;

MsgView status_msg;
Clock clk;



void show_victory_message(int piece) {
    char msg[128];
    
    sprintf(msg, "%s Win!", piece == CROSS ? "Cross" : "Ball");
    mv_show_text(&status_msg, msg, ALIGN_CENTER);
}
 

void show_draw_message() {
    mv_show_text(&status_msg, "Draw!", ALIGN_CENTER);
}



bool screen_to_board(int mx, int my, Point *p) {
    if (mx < BOARD_X || mx >= BOARD_X + BOARD_WIDTH ||
        my < BOARD_Y || my >= BOARD_Y + BOARD_HEIGHT) return false;
    
    int cell_size_x = BOARD_WIDTH/SIDE;
    int cell_size_y = BOARD_HEIGHT/SIDE;

    
    p-> x  = (mx-BOARD_X)/cell_size_x;
    p-> y  = (my-BOARD_Y)/cell_size_y;
    return true;
}

void mouse_handler(MouseEvent me) {    
    
    if (me.type == MOUSE_BUTTON_EVENT && me.state == BUTTON_CLICK) {
        if (game_over) {
            graph_exit();
            return;
        }
        else {
            Point bp;
             
            
            if ( !screen_to_board(me.x, me.y, &bp)) {
                return;  // cannot play
            }
             
            int res = ttt_play(&theBoard, bp.x, bp.y, turn);
            if (res != PLAY_INVALID) {
                draw_piece(bp.x, bp.y, turn);
                switch(res) {
                    case PLAY_WIN:
                        show_victory_message(turn);
                        game_over = true;
                        break;
                    case PLAY_DRAW:
                        show_draw_message();
                        game_over = true;
                    case PLAY_NO_WIN:
                        turn = (turn == CROSS) ? BALL : CROSS;
                        break;
                    default:
                        break;
                }
            }
                    
                
        }
        
    }
}


void timer_handler() {
    if (!game_over) {
        clk_tick(&clk);
        clk_show(&clk);
    }
}



void prepare_game() {
     // draw background
    graph_rect(0,0, WINDOW_WIDTH, WINDOW_HEIGHT,  BOARD_COLOR, true);
    
    ttt_create_board(&theBoard);
    ttt_draw_board();    
     
    mv_create(&status_msg, STATUS_MSG_X, STATUS_MSG_Y, STATUS_MSG_CHARS, STATUS_MSG_FONT, STATUS_MSG_TC, STATUS_MSG_BC);
    mv_show_text(&status_msg, "Game Started", ALIGN_CENTER);
    
    clk_create(&clk, CLOCK_X, CLOCK_Y, CLOCK_FONT, CLOCK_CLR, CLOCK_BACK_COLOR);
    clk_show(&clk);
    
    turn = CROSS;
     
}
    
    

int main() {
     
    graph_init2("Tic Tac Toe", WINDOW_WIDTH, WINDOW_HEIGHT);
    prepare_game();
        
    graph_regist_mouse_handler(mouse_handler);
    graph_regist_timer_handler(timer_handler, TIMER_PERIOD);
    
    graph_start();
     
}
    
