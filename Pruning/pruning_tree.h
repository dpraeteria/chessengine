#pragma once
#ifndef PRUNING_TREE_H_INCLUDED
#define PRUNING_TREE_H_INCLUDED

#include <limits>
#include "../Evaluation/protos.h"
#include "Board.h"
#include "Node.h"

constexpr int max_depth = 5;
constexpr auto inf = std::numeric_limits<int>::min();

class PruningTree {
public:
	/// <summary>
	/// 가지치기 트리를 초기화하는 역할을 한다.
	/// <para>
	///	 생성자와 동일한 역할을 하나,
	///	 root가 포인터이기에 별도의 함수를 사용하도록 한다.
	/// </para>
	/// </summary>
	void init() {
		root = Node();
	}

	/// <summary>
	/// 다음에 이어질 최적의 수를 반환한다.
	/// </summary>
	/// <returns> 계산된 최적의 수 </returns>
	Move get_nxt_move() {
		int max_val = -inf;	Move max_move;
		int min_val = +inf;	Move min_move;
		//root의 모든 가능성을 root에 자식으로 추가하기
		for (Move move : Board(root.fen).movable_cases()) {
			Node cld = root.make_moved_node(move); //root는 모든 가능성을 자식으로 가져야 한다!
			root.cld.push_back(cld);
			//cld를 기준으로 탐색을 시작한다.
			int val = finding_n_pruning(cld, max_depth, -inf, inf);
			//탐색 결과를 최소, 최댓값과 비교해 저장한다.
			if (max_val < val) {
				max_val = val;
				max_move = move;
			}
			if (min_val > val) {
				min_val = val;
				min_move = move;
			}
		}

		if (root.get_turn() == WHITE)
			return max_move;
		else
			return min_move;
	}

	/// <summary>
	/// 상대가 어떤 수를 두었는지를 입력받는다.
	/// <para>
	///	 이 함수의 내부에서 가지치기 탐색 함수를 실행시킨다.
	/// </para>
	/// </summary>
	/// <param name="move"> 상대가 둔 수 </param>
	void set_new_board(string fen) {
		//어차피 이전에 계산한 것들을 활용한다 할지라도,
		//  최고 깊이까지 탐색해야 하므로 재활용은 큰 의미가 없다.
		//그러니 편의를 위해 root를 지워버리고 시작한다.
		root.~Node();
		root = Node(fen);
	}

	/// <summary>
	/// 가지치기 트리에 할당된 모든 메모리를 해제한다.
	/// </summary>
	void release() {
		root.~Node();
	}

private:
	Node root;

	//! 시작 노드와 깊이, alpha 및 beta는 반드시 필요함.
	//! 그러나 max_player는 fen에 포함된 차례 정보로 대체할 수 있지 않을까?
	int finding_n_pruning(Node& node, int depth, int alpha/*= -inf*/, int beta/*= inf*/) {
		const string& fen = node.fen;
		const Board now_board = Board(fen);

		//moves = 움직임 경우의 수 벡터
		vector<Move> moves = now_board.movable_cases();

		//목표한 최고 깊이에 도달했거나, 리프 노드라면 평가함수의 값을 반환한다.
		if (depth == 0 || moves.size() == 0 || now_board.checkmate())
			return eval(fen.c_str()); //!Node에 평가함수 값 반환 함수를 내장시키는 것 고려

		if (node.get_turn() == WHITE) {
			int v = -inf;
			for (Move move : moves) {
				Node cld = node.make_moved_node(move);
				const Side check_side = Board(cld.fen).check();
				//백이 수를 분 결과로 백이 체크가 되었다면 continue.
				if (check_side == WHITE ||
					check_side == GREY)
					continue;

				v = std::max(v, finding_n_pruning(cld, depth - 1, alpha, beta));
				alpha = std::max(alpha, v);
				if (alpha >= beta)
					break;

				if (find(node.cld.begin(), node.cld.end(), cld) == node.cld.end())
					node.cld.push_back(cld);
			}
			return v;
		}
		else {
			int v = inf;
			for (Move move : moves) {
				Node cld = node.make_moved_node(move);
				const Side check_side = Board(cld.fen).check();
				//백이 수를 분 결과로 백이 체크가 되었다면 continue.
				if (check_side == BLACK ||
					check_side == GREY)
					continue;

				v = std::max(v, finding_n_pruning(cld, depth - 1, alpha, beta));
				beta = std::max(beta, v);
				if (alpha >= beta)
					break;

				if (find(node.cld.begin(), node.cld.end(), cld) == node.cld.end())
					node.cld.push_back(cld);
			}
			return v;
		}
	}

};

#endif // !PRUNING_TREE_H_INCLUDED