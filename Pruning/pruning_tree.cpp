#include <iostream>
#include "pruning_tree.h"
#include "Board.h"

int main(void) {
	//string fens[] = {
	//	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0",
	//	"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR - - - 0 0",
	//	"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
	//	"r1b1k1nr/p2p1pNp/n2B4/1p1NP2P/6P1/3P1Q2/P1P1K3/q5b1 - - - 0 0",
	//	"8/8/8/4p1K1/2k1P3/8/8/8 - - - 0 0",
	//	"7P/6P1/5P2/4P3/3P4/2P5/1P6/P7 - - - - -",
	//}, fen = fens[2];
	//
	//Board a = Board(fen);
	//
	//using std::cout;
	////a.print_movable_cases(WHITE);
	//cout << eval(fen.c_str()) << '\n';
	////cout << '\n';
	////cout << fen << '\n';
	////cout << a.to_fen() << '\n';

	/*
		< how to play CHESS >
	
	Ű�� �̵� : WASD
	��ǥ ���� : SPACE�� �̷�����,
	��� : Q
	���� : ESC
	
	��� ���� ���ʰ� ����� �̷�����,
	    �� �ȿ����� �⹰ ���ð� ��ǥ ��ǥ�� ������ ����� �̷�����.
	
	�Է��� �߸��Ǹ� �⹰ ���ú��� �ٽ� ���۵ȴ�.
	*/
	Board::GAME("4k3/8/8/8/8/8/8/4K2R w K - 0 0");

	return 0;
}