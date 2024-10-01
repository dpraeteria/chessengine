#include <stdio.h>
#include "Board.h"

int main(void) {
	



	string fen = "r1b1k1nr/p2p1pNp/n2B4/1p1NP2P/6P1/3P1Q2/P1P1K3/q5b1";
	Board b = Board(fen);
	b.print();

	Board a = b;
	a.print();

	b.set_on(Coord(RANK_8, FILE_A), ' ');
	b.print();
	a.print();
	return 0;
}