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
		//root의 모든 가능성을 root에 자식으로 추가하기
		Board root_board = Board(fen);
		vector<Move> moves = root_board.movable_cases();
		for (Move move : moves) {
			string cld = root_board.make_moved_board(move).to_fen();
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
		Board root_board = Board(fen);
		vector<Move> moves = root_board.movable_cases();
		for (Move move : moves) {
			string cld = root_board.make_moved_board(move).to_fen();
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
//! 시작 노드와 깊이, alpha 및 beta는 반드시 필요함.
//! 그러나 max_player는 fen에 포함된 차례 정보로 대체할 수 있지 않을까?
int PruningTree::finding_n_pruning(string fen, int depth, int alpha, int beta) {
	const Board now_board = Board(fen);
	
	//moves = 움직임 경우의 수 벡터
	vector<Move> moves = now_board.movable_cases();
	
	//목표한 최고 깊이에 도달했거나, 리프 노드라면 평가함수의 값을 반환한다.
	//체크메이트 확인 없이도 잘 돌아가리라 생각하긴 하지만, 그래도 체크메이트 확인을 못하니 아쉽다.
	if (depth == 0 || moves.size() == 0/* || now_board.checkmate()*/)
		return eval(fen.c_str()); //!Node에 평가함수 값 반환 함수를 내장시키는 것 고려
	
	Board par = Board(fen);
	if (get_turn(fen) == WHITE) {
		int v = -inf;
		for (Move move : moves) {
			string cld = par.make_moved_board(move).to_fen();
			const Side check_side = Board(cld).check();
			//백이 수를 분 결과로 백이 체크가 되었다면 continue.
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
			//백이 수를 분 결과로 백이 체크가 되었다면 continue.
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
