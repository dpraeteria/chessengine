#pragma once
#ifndef COORD_H_INCLUDED
#define COORD_H_INCLUDED

/// <summary>
///	행을 의미하는 Rank의 열거형이다.
/// <para>
///		Nullable이며, 그 값은 -1이다.
///		증감 연산자만을 정의하였다.
/// </para>
/// </summary>
enum Rank : char {
	RANK_1, RANK_2, RANK_3, RANK_4,
	RANK_5, RANK_6, RANK_7, RANK_8,
	RANK_NON = -1,
};
inline Rank& operator++(Rank& rank) { return rank = Rank((char)rank + 1); }
inline Rank& operator--(Rank& rank) { return rank = Rank((char)rank - 1); }
inline Rank operator+(Rank& rank, int i) { return Rank((char)rank + i); }
inline Rank operator-(Rank& rank, int i) { return Rank((char)rank - i); }

/// <summary>
///	열을 의미하는 File의 열거형이다.
/// <para>
///		Nullable이며, 그 값은 -1이다.
///		증감 연산자만을 정의하였다.
/// </para>
/// </summary>
enum File : char {
	FILE_A, FILE_B, FILE_C, FILE_D,
	FILE_E, FILE_F, FILE_G, FILE_H,
	FILE_NON = -1,
};
inline File& operator++(File& rank) { return rank = File((char)rank + 1); }
inline File& operator--(File& rank) { return rank = File((char)rank - 1); }
inline File operator+(File& file, int i) { return File((char)file + i); }
inline File operator-(File& file, int i) { return File((char)file - i); }


/// <summary>
///	행과 열을 좌표화시킨 구조체이다.
///	<para> Rank : 행 </para>
///	<para> File : 열 </para>
/// <remarks>
///		생성자가 값의 범위를 보장해주지 않음에 주의하라.
/// </remarks>
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