#include <limits>
#include "pruning_tree.h"
#include "Board.h"
#include "../Evaluation/protos.h"


constexpr int max_depth = 2;
constexpr auto inf = std::numeric_limits<int>::max();


Move PruningTree::get_nxt_move(const Board& root_board) {
	if (root_board.get_turn() == White) {
		int max_val = -inf;
		Move max_move;
		//root�� ��� ���ɼ��� root�� �ڽ����� �߰��ϱ�
		vector<Move> moves = root_board.movable_cases();
		for (Move move : moves) {
			Board cld = root_board.make_moved_board(move);
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
		vector<Move> moves = root_board.movable_cases();
		for (Move move : moves) {
			Board cld = root_board.make_moved_board(move);
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
int PruningTree::finding_n_pruning(const Board& par, int depth, int alpha, int beta) {
	//moves = ������ ����� �� ����
	vector<Move> moves = par.movable_cases();
	
	//��ǥ�� �ְ� ���̿� �����߰ų�, ���� ����� ���Լ��� ���� ��ȯ�Ѵ�.
	if (depth == 0 || moves.size() == 0)
		return eval(par.to_fen().c_str()); //eval() ����ȭ �Ϸ�Ǹ� �� �κ� ����
	
	if (par.get_turn() == White) {
		int v = -inf;
		for (Move move : moves) {
			const Board cld = par.make_moved_board(move);
			const Side check_side = cld.check();
			//���� ���� �� ����� ���� üũ�� �Ǿ��ٸ� continue.
			if (check_side == White ||
				check_side == Grey)
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
			const Board cld = par.make_moved_board(move);
			const Side check_side = cld.check();
			//���� ���� �� ����� ���� üũ�� �Ǿ��ٸ� continue.
			if (check_side == Black ||
				check_side == Grey)
				continue;
	
			v = std::min(v, finding_n_pruning(cld, depth - 1, alpha, beta));
			beta = std::min(beta, v);
			if (alpha >= beta)
				break;
		}
		return v;
	}
}
