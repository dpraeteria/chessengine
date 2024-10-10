#include "../Pruning/Board.h"

/* eval.c */
int eval(const char* fen);
int eval(const Board& board);
int eval_light_pawn(int sq);
int eval_dark_pawn(int sq);
int eval_light_king(int sq);
int eval_lkp(int f);
int eval_dark_king(int sq);
int eval_dkp(int f);

/* main.c */
//int main();
//void print_board();
//void print_result();
