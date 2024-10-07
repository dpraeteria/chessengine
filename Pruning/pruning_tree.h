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
	/// ����ġ�� Ʈ���� �ʱ�ȭ�ϴ� ������ �Ѵ�.
	/// <para>
	///	 �����ڿ� ������ ������ �ϳ�,
	///	 root�� �������̱⿡ ������ �Լ��� ����ϵ��� �Ѵ�.
	/// </para>
	/// </summary>
	void init() {
		root = Node();
	}

	/// <summary>
	/// ������ �̾��� ������ ���� ��ȯ�Ѵ�.
	/// </summary>
	/// <returns> ���� ������ �� </returns>
	Move get_nxt_move() {
		int max_val = -inf;	Move max_move;
		int min_val = +inf;	Move min_move;
		//root�� ��� ���ɼ��� root�� �ڽ����� �߰��ϱ�
		for (Move move : Board(root.fen).movable_cases()) {
			Node cld = root.make_moved_node(move); //root�� ��� ���ɼ��� �ڽ����� ������ �Ѵ�!
			root.cld.push_back(cld);
			//cld�� �������� Ž���� �����Ѵ�.
			int val = finding_n_pruning(cld, max_depth, -inf, inf);
			//Ž�� ����� �ּ�, �ִ񰪰� ���� �����Ѵ�.
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
	/// ��밡 � ���� �ξ������� �Է¹޴´�.
	/// <para>
	///	 �� �Լ��� ���ο��� ����ġ�� Ž�� �Լ��� �����Ų��.
	/// </para>
	/// </summary>
	/// <param name="move"> ��밡 �� �� </param>
	void set_new_board(string fen) {
		//������ ������ ����� �͵��� Ȱ���Ѵ� ������,
		//  �ְ� ���̱��� Ž���ؾ� �ϹǷ� ��Ȱ���� ū �ǹ̰� ����.
		//�׷��� ���Ǹ� ���� root�� ���������� �����Ѵ�.
		root.~Node();
		root = Node(fen);
	}

	/// <summary>
	/// ����ġ�� Ʈ���� �Ҵ�� ��� �޸𸮸� �����Ѵ�.
	/// </summary>
	void release() {
		root.~Node();
	}

private:
	Node root;

	//! ���� ���� ����, alpha �� beta�� �ݵ�� �ʿ���.
	//! �׷��� max_player�� fen�� ���Ե� ���� ������ ��ü�� �� ���� ������?
	int finding_n_pruning(Node& node, int depth, int alpha/*= -inf*/, int beta/*= inf*/) {
		const string& fen = node.fen;
		const Board now_board = Board(fen);

		//moves = ������ ����� �� ����
		vector<Move> moves = now_board.movable_cases();

		//��ǥ�� �ְ� ���̿� �����߰ų�, ���� ����� ���Լ��� ���� ��ȯ�Ѵ�.
		if (depth == 0 || moves.size() == 0 || now_board.checkmate())
			return eval(fen.c_str()); //!Node�� ���Լ� �� ��ȯ �Լ��� �����Ű�� �� ���

		if (node.get_turn() == WHITE) {
			int v = -inf;
			for (Move move : moves) {
				Node cld = node.make_moved_node(move);
				const Side check_side = Board(cld.fen).check();
				//���� ���� �� ����� ���� üũ�� �Ǿ��ٸ� continue.
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
				//���� ���� �� ����� ���� üũ�� �Ǿ��ٸ� continue.
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