#pragma once
#ifndef PRUNING_TREE_H_INCLUDED
#define PRUNING_TREE_H_INCLUDED

#include "Node.h"

class PruningTree {
public:
	/// <summary>
	/// 다음에 이어질 최적의 수를 반환한다.
	/// </summary>
	/// <returns> 계산된 최적의 수 </returns>
	Move get_nxt_move(const Board& board);

private:
	/// <summary>
	/// 알파-베타 가지치기 트리
	/// </summary>
	/// <param name="fen"> 탐색을 시작하는 노드 </param>
	/// <param name="depth"> 탐색 넓이 </param>
	/// <param name="alpha"> 휴리스틱 범위의 최솟값 </param>
	/// <param name="beta"> 휴리스틱 범위의 최댓값 </param>
	/// <returns> 탐색 결과, 최적인 휴리스틱 값 </returns>
	int finding_n_pruning(const Board& board, int depth, int alpha, int beta);

};

#endif // !PRUNING_TREE_H_INCLUDED