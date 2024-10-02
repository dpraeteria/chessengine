#pragma once
#ifndef PRUNING_TREE_H_INCLUDED
#define PRUNING_TREE_H_INCLUDED

#include <vector>
#include <limits>
#include "../Evaluation/protos.h"
#include "Board.h"
#include "Node.h"

class PruningTree {
public:
    //초기화
    void init() {
        root = new Node(/*...*/);
    }

    //다음 최적의 수 반환
    Move get_nxt_move() {
        return Move();
    }

    //현재 상대의 수 입력
    void set_new_move(Move move) {
        ;
    }

    //종료
    void release() {
        root->~Node();
    }

private:
    Node* root = nullptr;

    int finding_n_pruning(Node* node, int depth, int alpha/*= -inf*/, int beta/*= inf*/, Side max_player/*= WHITE*/) {
        const string& fen = node->get_fen();
        const Board now_board = Board(fen);
        //moves = 움직임 경우의 수 벡터
        vector<Move> moves = now_board.movable_cases(max_player); //나중에 ~.movable_... 까지 클래스 안에 넣는 것 고려

        //목표한 최고 깊이에 도달했거나, 자식 노드(움직일 수 있는 수)가 0개인 리프 노드라면 평가함수의 값을 반환한다.
        if (depth == 0 || moves.size() == 0)
            return eval(fen.c_str()); //평가함수 값 반환을 내장시키는 것 고려

        constexpr auto inf = std::numeric_limits<double>::infinity();
        if (max_player == WHITE) {
            int v = -inf;
            //단순히 move에 의한 것들을 계산하는 대신, node가 이미 가지고 있는 자식들도 사용해야 한다.
            for (Move move : moves) {
                Node* cld = new Node(move, now_board.apply_move(move).to_fen());
                //여기에 체크 판정 함수 사용.
                //  체크일 때에는 체크를 탈출 할 수 있는 수만을 사용하기
                //  체크 상황에서 "체크 진영 == 입력 진영"일 때는 다음 수에 게임이 끝남을 의미.
                //  그러니 이 상황에서는 바로 break로 탈출하도록.
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
            //위 내용 복붙으로 구현
            return v;
        }
    }

};

#endif // !PRUNING_TREE_H_INCLUDED