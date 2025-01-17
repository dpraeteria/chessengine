#pragma once
#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include "Coord.h"

/// <summary>
/// 기물의 움직임을 나타낸 구조체이다.
/// <para>
///		움직임은 시작 좌표와 끝 좌표로 이루어져 있으며,
///		시작 위치의 기물은 끝 위치로 움직일 수 을 전제로 한다.
/// </para>
/// </summary>
struct Move {
	Coord src;
	Coord dst;

	Move() : src(), dst() {}
	Move(const Coord& src_crd, const Coord& dst_crd)
		: src(src_crd), dst(dst_crd) {}

	bool operator==(const Move& other) const {
		if (src == other.src &&
			dst == other.dst)
			return true;
		else
			return false;
	}
};

#endif // !MOVE_H_INCLUDED