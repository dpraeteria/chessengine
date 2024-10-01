#pragma once
#ifndef COORD_H_INCLUDED
#define COORD_H_INCLUDED

enum Rank : char {
	RANK_1, RANK_2, RANK_3, RANK_4,
	RANK_5, RANK_6, RANK_7, RANK_8,
	RANK_NON = -1,
};
inline Rank& operator++(Rank& rank) { return rank = Rank((char)rank + 1); }
inline Rank& operator--(Rank& rank) { return rank = Rank((char)rank - 1); }

enum File : char {
	FILE_A, FILE_B, FILE_C, FILE_D,
	FILE_E, FILE_F, FILE_G, FILE_H,
	FILE_NON = -1,
};
inline File& operator++(File& rank) { return rank = File((char)rank + 1); }
inline File& operator--(File& rank) { return rank = File((char)rank - 1); }


/// <summary>
/// 행과 열을 좌표화시킴.
/// <remark>
/// Rank = 행,
/// File = 열
/// </remark>
/// </summary>
struct Coord {
	Rank rank; //행을 의미하며, 숫자로 표현된다.
	File file; //열을 의미하며, 알파벳으로 표현된다.
	Coord() : rank(RANK_NON), file(FILE_NON) {}
	Coord(Rank rank, File file)
		: rank(rank), file(file) {}
	Coord(char rank, char file)
		: rank(Rank(rank)), file(File(file)) {}
};

#endif // !COORD_H_INCLUDED