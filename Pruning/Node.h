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
		last_move = Move();
		fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
		par = nullptr;
	}
	~Node() {
		for (auto& ptr : cld) {
			ptr->~Node();
			delete ptr;
		}
	}

	string get_fen() {
		return fen;
	}
	vector<Node*> get_cld() {
		return cld;
	}
	bool is_leaf() {
		return cld.size() == 0;
	}

private:
	Move last_move; //이거 쓸모없을 확률이 높다. 그럼 나중에 지우는걸로.
	string fen;

	Node* par;
	vector<Node*> cld;
};

#endif // !NODE_H_INCLUDED