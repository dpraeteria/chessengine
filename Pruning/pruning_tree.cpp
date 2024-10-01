#include <stdio.h>
#include "Board.h"

int main(void) {
	printf("Hello world!\n");
	printf("TEST2\n");



	string fen = "r1b1k1nr/p2p1pNp/n2B4/1p1NP2P/6P1/3P1Q2/P1P1K3/q5b1 - - - - -";
	//string fen = "8/8/8/4p1K1/2k1P3/8/8/8 b - - 0 1";
	Board a = Board(fen);
	a.print();
	std::cout << string(32, ' ') << '\n';

	a.print_movable_cases(WHITE);

	return 0;
}