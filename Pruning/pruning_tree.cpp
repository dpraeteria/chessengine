#include <stdio.h>
#include "Board.h"
#include "../Evaluation/protos.h"

//extern int eval(const char* fen);

int main(void) {
	//string fen = "r1b1k1nr/p2p1pNp/n2B4/1p1NP2P/6P1/3P1Q2/P1P1K3/q5b1";
	//string fen = "8/8/8/4p1K1/2k1P3/8/8/8";
	//string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	string fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR";
	Board a = Board(fen);

	//a.print_movable_cases(WHITE);
	std::cout << eval(fen.c_str()) << '\n';

	return 0;
}