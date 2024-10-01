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
	;

private:
	Move last_move;
	string fen;

	Node* par;
	vector<Node*> cld;
};

#endif // !NODE_H_INCLUDED