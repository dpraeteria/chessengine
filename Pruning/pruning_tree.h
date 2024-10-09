#pragma once
#ifndef PRUNING_TREE_H_INCLUDED
#define PRUNING_TREE_H_INCLUDED

#include "Node.h"

class PruningTree {
public:
	/// <summary>
	/// ������ �̾��� ������ ���� ��ȯ�Ѵ�.
	/// </summary>
	/// <returns> ���� ������ �� </returns>
	Move get_nxt_move(const Board& board);

private:
	/// <summary>
	/// ����-��Ÿ ����ġ�� Ʈ��
	/// </summary>
	/// <param name="fen"> Ž���� �����ϴ� ��� </param>
	/// <param name="depth"> Ž�� ���� </param>
	/// <param name="alpha"> �޸���ƽ ������ �ּڰ� </param>
	/// <param name="beta"> �޸���ƽ ������ �ִ� </param>
	/// <returns> Ž�� ���, ������ �޸���ƽ �� </returns>
	int finding_n_pruning(const Board& board, int depth, int alpha, int beta);

};

#endif // !PRUNING_TREE_H_INCLUDED