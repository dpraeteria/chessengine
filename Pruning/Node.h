#pragma once
#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <vector>
#include <string>
#include "Board.h"
#include "Move.h"
using std::string;
using std::vector;

class Node {
public:
	Node() : fen(default_fen) {}
	Node(string fen)
		: fen(fen) {}
	/// <summary>
	/// Node의 소멸자.
	/// <para>
	///		실행하면 자식으로 가지고 있는 Node까지 모두 소멸시킨다.
	/// </para>
	/// </summary>
	~Node() { //소멸자가 올바르게 동작하는지에 대해서는 검증 필요. 이는 가지치기 트리를 개발하며 이뤄질 예정임.
		for (Node node : cld)
			node.~Node();
		//delete this;
	}
	bool operator==(const Node& other) const {
		if (fen != other.fen)
			return false;
		return true;
	}

	/// <summary>
	/// 저장된 FEN의 차례를 반환하는 함수이다.
	/// </summary>
	/// <returns> FEN에 기록된 차례 </returns>
	Side get_turn() const {
		int i;
		for (i = 0; fen[i] != ' '; ++i);
		for (i = 0; fen[i] == ' '; ++i);
		char turn = fen[i];
		switch (turn) {
		case 'W': case 'w': return WHITE;
		case 'B': case 'b': return BLACK;
		default:
			return EMPTY;
		}
	}
	/// <summary>
	/// move라는 수를 둔 결과의 Node를 반환하는 함수이다.
	/// <para>
	///		*this를 복제해 apply_move()를 적용하는 방식이다.
	/// </para>
	/// </summary>
	/// <param name="move"> 기물의 움직임 </param>
	/// <returns> 움직임을 적용한 결과 </returns>
	Node make_moved_node(Move move) const {
		string new_fen = Board(fen).make_moved_board(move).to_fen();
		Node cld = Node(new_fen);
		return cld;
	}

//private:
	string fen;
	vector<Node> cld;

};

#endif // !NODE_H_INCLUDED