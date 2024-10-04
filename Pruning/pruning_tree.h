#pragma once
#ifndef PRUNING_TREE_H_INCLUDED
#define PRUNING_TREE_H_INCLUDED

#include <limits>
#include "../Evaluation/protos.h"
#include "Board.h"
#include "Node.h"

class PruningTree {
public:
    /// <summary>
    /// ����ġ�� Ʈ���� �ʱ�ȭ�ϴ� ������ �Ѵ�.
    /// <para>
    ///     �����ڿ� ������ ������ �ϳ�,
    ///     root�� �������̱⿡ ������ �Լ��� ����ϵ��� �Ѵ�.
    /// </para>
    /// </summary>
    void init() {
        root = new Node();
    }

    /// <summary>
    /// ������ �̾��� ������ ���� ��ȯ�Ѵ�.
    /// </summary>
    /// <returns> ���� ������ �� </returns>
    Move get_nxt_move() {
        //�̿�
        return Move();
    }

    /// <summary>
    /// ��밡 � ���� �ξ������� �Է¹޴´�.
    /// </summary>
    /// <param name="move"> ��밡 �� �� </param>
    void set_new_move(Move move) {
        //�̿�
    }

    /// <summary>
    /// ����ġ�� Ʈ���� �Ҵ�� ��� �޸𸮸� �����Ѵ�.
    /// </summary>
    void release() {
        root->~Node();
    }

private:
    Node* root = nullptr;

    int finding_n_pruning(Node* node, int depth, int alpha/*= -inf*/, int beta/*= inf*/, Side max_player/*= WHITE*/) {
        const string& fen = node->get_fen();
        const Board now_board = Board(fen);
        //moves = ������ ����� �� ����
        vector<Move> moves = now_board.movable_cases(max_player); //���߿� ~.movable_... ���� Ŭ���� �ȿ� �ִ� �� ����

        //��ǥ�� �ְ� ���̿� �����߰ų�, �ڽ� ���(������ �� �ִ� ��)�� 0���� ���� ����� ���Լ��� ���� ��ȯ�Ѵ�.
        if (depth == 0 || moves.size() == 0)
            return eval(fen.c_str()); //���Լ� �� ��ȯ�� �����Ű�� �� ����

        constexpr auto inf = std::numeric_limits<int>::min();
        if (max_player == WHITE) {
            int v = -inf;
            //�ܼ��� move�� ���� �͵��� ����ϴ� ���, node�� �̹� ������ �ִ� �ڽĵ鵵 ����ؾ� �Ѵ�.
            for (Move move : moves) {
                Node* cld = new Node(move, now_board.make_moved_board(move).to_fen());
                //���⿡ üũ ���� �Լ� ���.
                //  üũ�� ������ üũ�� Ż�� �� �� �ִ� ������ ����ϱ�
                //  üũ ��Ȳ���� "üũ ���� == �Է� ����"�� ���� ���� ���� ������ ������ �ǹ�.
                //  �׷��� �� ��Ȳ������ �ٷ� break�� Ż���ϵ���.
                v = std::max(v, finding_n_pruning(cld, depth - 1, alpha, beta, BLACK));
                alpha = std::max(alpha, v);
                if (alpha >= beta)
                    break;
                node->get_cld().push_back(cld);
            }
            return v;
        }
        else {
            int v = inf;
            //�� ���� �������� ����
            return v;
        }
    }

};

#endif // !PRUNING_TREE_H_INCLUDED