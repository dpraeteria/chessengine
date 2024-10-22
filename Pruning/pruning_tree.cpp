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
		//root의 모든 가능성을 root에 자식으로 추가하기
		vector<Move> moves = root_board.movable_cases();
		for (Move move : moves) {
			Board cld = root_board.make_moved_board(move);
			//cld를 기준으로 탐색을 시작한다.
			int val = finding_n_pruning(cld, max_depth, -inf, inf);
			//탐색 결과를 최소, 최댓값과 비교해 저장한다.
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
		//root의 모든 가능성을 root에 자식으로 추가하기
		vector<Move> moves = root_board.movable_cases();
		for (Move move : moves) {
			Board cld = root_board.make_moved_board(move);
			//cld를 기준으로 탐색을 시작한다.
			int val = finding_n_pruning(cld, max_depth, -inf, inf);
			//탐색 결과를 최소, 최댓값과 비교해 저장한다.
			if (min_val > val) {
				min_val = val;
				min_move = move;
			}
		}
		return min_move;
	}
}
int PruningTree::finding_n_pruning(const Board& par, int depth, int alpha, int beta) {
	//moves = 움직임 경우의 수 벡터
	vector<Move> moves = par.movable_cases();
	
	//목표한 최고 깊이에 도달했거나, 리프 노드라면 평가함수의 값을 반환한다.
	if (depth == 0 || moves.size() == 0)
		return eval(par);
	
	if (par.get_turn() == White) {
		int v = -inf;
		for (Move move : moves) {
			const Board cld = par.make_moved_board(move);
			const Side check_side = cld.check();
			//백이 수를 분 결과로 백이 체크가 되었다면 continue.
			if (check_side == White ||
				check_side == Grey)
				continue;
	
			if (alpha >= beta)
				break;
			v = std::max(v, finding_n_pruning(cld, depth - 1, alpha, beta));
			alpha = std::max(alpha, v);
		}
		return v;
	}
	else {
		int v = inf;
		for (Move move : moves) {
			const Board cld = par.make_moved_board(move);
			const Side check_side = cld.check();
			//백이 수를 분 결과로 백이 체크가 되었다면 continue.
			if (check_side == Black ||
				check_side == Grey)
				continue;
	
			if (alpha >= beta)
				break;
			v = std::min(v, finding_n_pruning(cld, depth - 1, alpha, beta));
			beta = std::min(beta, v);
		}
		return v;
	}
}
