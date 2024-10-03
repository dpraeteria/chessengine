#pragma once
#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <vector>
#include <string>
#include "Move.h"
using std::string;
using std::vector;

class Node {
public:
	Node() {
		*this = Node(Move(), "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
	}
	Node(Move move, string fen)
		: last_move(move), fen(fen) {}
	/// <summary>
	/// Node의 소멸자.
	/// <para>
	///		실행하면 자식으로 가지고 있는 Node까지 모두 소멸시킨다.
	/// </para>
	/// </summary>
	~Node() { //올바르게 동작하는지에 대해서는 검증 필요.
		for (auto& ptr : cld)
			ptr->~Node();
		delete this;
	}

	string get_fen() {
		return fen;
	}
	vector<Node*> get_cld() {
		return cld;
	}

private:
	Move last_move; //이거 쓸모없을 확률이 높다. 그럼 나중에 지우는걸로.
	string fen;

	vector<Node*> cld;
};

#endif // !NODE_H_INCLUDED