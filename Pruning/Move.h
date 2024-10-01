#pragma once
#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include "Coord.h"

struct Move {
	char piece; //FEN 방식을 따라 움직인 기물의 종류를 나타낸다.

	Coord src_coord;
	Coord dst_coord;
};

#endif // !MOVE_H_INCLUDED