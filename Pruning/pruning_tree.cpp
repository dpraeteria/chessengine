#include <limits>
#include "pruning_tree.h"
#include "Board.h"
#include "../Evaluation/protos.h"


constexpr int max_depth = 2;
constexpr auto inf = std::numeric_limits<int>::max();


static Side get_turn(string fen) {
	volatile int i = 0;
	for (; fen[i] != ' '; ++i);
	for (; fen[i] == ' '; ++i);
	char turn = fen[i];
	switch (turn) {
	case 'W': case 'w': return WHITE;
	case 'B': case 'b': return BLACK;
	default:
		return EMPTY;
	}
}
Move PruningTree::get_nxt_move(string fen) {
	if (get_turn(fen) == WHITE) {
		int max_val = -inf;
		Move max_move;
		//root�� ��� ���ɼ��� root�� �ڽ����� �߰��ϱ�
		Board root_board = Board(fen);
		vector<Move> moves = root_board.movable_cases();
		for (Move move : moves) {
			string cld = root_board.make_moved_board(move).to_fen();
			//cld�� �������� Ž���� �����Ѵ�.
			int val = finding_n_pruning(cld, max_depth, -inf, inf);
			//Ž�� ����� �ּ�, �ִ񰪰� ���� �����Ѵ�.
			if (max_val < val) {
				max_val = val;
				max_move = move;
			}
		}
		return max_move;
	}
	else {
		int min_val = +inf;
		Move min_move;
		//root�� ��� ���ɼ��� root�� �ڽ����� �߰��ϱ�
		Board root_board = Board(fen);
		vector<Move> moves = root_board.movable_cases();
		for (Move move : moves) {
			string cld = root_board.make_moved_board(move).to_fen();
			//cld�� �������� Ž���� �����Ѵ�.
			int val = finding_n_pruning(cld, max_depth, -inf, inf);
			//Ž�� ����� �ּ�, �ִ񰪰� ���� �����Ѵ�.
			if (min_val > val) {
				min_val = val;
				min_move = move;
			}
		}
		return min_move;
	}
}
//! ���� ���� ����, alpha �� beta�� �ݵ�� �ʿ���.
//! �׷��� max_player�� fen�� ���Ե� ���� ������ ��ü�� �� ���� ������?
int PruningTree::finding_n_pruning(string fen, int depth, int alpha, int beta) {
	const Board now_board = Board(fen);
	
	//moves = ������ ����� �� ����
	vector<Move> moves = now_board.movable_cases();
	
	//��ǥ�� �ְ� ���̿� �����߰ų�, ���� ����� ���Լ��� ���� ��ȯ�Ѵ�.
	//üũ����Ʈ Ȯ�� ���̵� �� ���ư����� �����ϱ� ������, �׷��� üũ����Ʈ Ȯ���� ���ϴ� �ƽ���.
	if (depth == 0 || moves.size() == 0/* || now_board.checkmate()*/)
		return eval(fen.c_str()); //!Node�� ���Լ� �� ��ȯ �Լ��� �����Ű�� �� ���
	
	Board par = Board(fen);
	if (get_turn(fen) == WHITE) {
		int v = -inf;
		for (Move move : moves) {
			string cld = par.make_moved_board(move).to_fen();
			const Side check_side = Board(cld).check();
			//���� ���� �� ����� ���� üũ�� �Ǿ��ٸ� continue.
			if (check_side == WHITE ||
				check_side == GREY)
				continue;
	
			v = std::max(v, finding_n_pruning(cld, depth - 1, alpha, beta));
			alpha = std::max(alpha, v);
			if (alpha >= beta)
				break;
		}
		return v;
	}
	else {
		int v = inf;
		for (Move move : moves) {
			string cld = par.make_moved_board(move).to_fen();
			const Side check_side = Board(cld).check();
			//���� ���� �� ����� ���� üũ�� �Ǿ��ٸ� continue.
			if (check_side == BLACK ||
				check_side == GREY)
				continue;
	
			v = std::min(v, finding_n_pruning(cld, depth - 1, alpha, beta));
			beta = std::min(beta, v);
			if (alpha >= beta)
				break;
		}
		return v;
	}
}
