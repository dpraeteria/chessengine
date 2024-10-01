#pragma once
#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

//! 디버그용 - print(), print_movable_cases() 함수에만 사용
#include <iostream>

#include <string>
#include <vector>
#include <cctype>
#include "Coord.h"
using std::string;
using std::vector;

enum Side : char { WHITE, BLACK, EMPTY };

/// <summary>
/// 체스 보드를 8*8의 형태로 저장하는 구조체.
/// <para>
///		용량이 크기 때문에 사용을 최소화해야 한다.
/// </para>
/// </summary>
class Board {
public:
	Board() {
		for (int i = 0; i < 64; ++i)
			board[i] = ' ';
	}
	Board(string FEN) {
		*this = Board();

		Rank rank = RANK_8;
		File file = FILE_A;
		for (char c : FEN) {
			if (c == ' ') {
				break;
			}
			else if (c == '/'); //c가 '/'인 경우는 무시한다.
			else if (isalpha(c)) {
				set_piece(Coord(rank, file), c);
				++file;
			}
			else if (isdigit(c)) {
				file = (File)(file + c - '0');	//file += c - '0'
			}
			rank = (Rank)(rank - file / 8);		//rank += file / 8;
			file = (File)(file % 8);			//file %= 8;
		}
	}
	
	/// <summary>
	/// 특정 좌표에 기물을 놓는 함수이다.
	/// </summary>
	/// <param name = 'coord'> 기물이 놓일 좌표 </param>
	/// <param name = 'piece'> 놓일 기물의 종류 </param>
	void set_piece(Coord coord, char piece) {
		//[0,64)를 [0,8), [8,16), [16,24), [24,32), [32,40), [40,48), [48,56), [56,64)의 8개의 구간으로 나누고,
		//이 구간들을 순서대로 쌓아서 8*8의 '2차원 배열'로 생각할 수 있다.
		//이때 Rank와 File은 각각 배열의 행과 열이 되고,
		//구간의 길이가 8이기 때문에 rank행 file열은 1차원 배열에서 8*rank+file 번째 위치와 같은 것이다.
		board[8 * coord.rank + coord.file] = piece;
	}

	/// <summary>
	/// 특정 좌표에서 기물의 종류를 반환하는 함수이다.
	/// </summary>
	/// <param name="coord"> 기물의 종류를 확인할 좌표 </param>
	/// <returns> 좌표 위 기물의 종류를 반환 </returns>
	char get_piece(Coord coord) const {
		//64칸중 랭크는 8의 배수.1랭크 올라갈 때마다 64칸 Array에서 index는 8 증가.(E.g | e파일 4랭크는 5행 4열 ( 5+4*8 ))
		return board[8 * coord.rank + coord.file];
	}

	/// <summary>
	/// 특정 좌표에서 기물의 진영을 반환하는 함수이다.
	/// </summary>
	/// <param name="coord"> 기물의 진영을 확인할 좌표 </param>
	/// <returns> 좌표 위 기물의 진영을 반환(enum Side 참고) </returns>
	Side get_side(Coord coord) const {
		if ('A' <= get_piece(coord) &&
			'Z' >= get_piece(coord))
			return WHITE;
		if ('a' <= get_piece(coord) &&
			'z' >= get_piece(coord))
			return BLACK;
		return EMPTY;
	}

	/// <summary>
	/// 현재 Board의 상태에서 진영에 따라 움직일 수 있는 모든 경우를 반환하는 함수이다.
	/// </summary>
	/// <param name="is_white_side"></param>
	/// <returns></returns>
	vector<Board> movable_boards(Side side) const {
		vector<Board> result;

		for (Rank rank = RANK_1; rank <= RANK_8; ++rank) {
			for (File file = FILE_A; file <= FILE_H; ++file) {
				Coord src_crd = Coord(rank, file);

				if (get_side(src_crd) == side);
				else
					continue;

				vector<Board> add;
				//놓인 말의 종류에 따라 움질임 경우의 수를 계산하는 함수를
				//	달리 구현하여 실행 및 변수 add에 대입한다.
				switch (get_piece(src_crd)) {
				case 'K': { add = movable_k(src_crd, side); break; }
				case 'Q': { add = movable_q(src_crd, side); break; }
				case 'R': { add = movable_r(src_crd, side); break; }
				case 'B': { add = movable_b(src_crd, side); break; }
				case 'N': { add = movable_n(src_crd, side); break; }
				case 'P': { add = movable_p(src_crd, side); break; }
				}
				//add의 요소들은 result에 추가된다.
				result.insert(result.end(),
					add.begin(), add.end());
			}
		}
		return result;
	}

	/// <summary>
	/// 디버그용 함수이다.
	/// Board에서의 기물의 위치를 8*8 격자판의 형태로 출력한다.
	/// <para>
	///		왼쪽 위가 a8, 오른쪽 아래가 h1이다.
	/// </para>
	/// </summary>
	void print() const {
		using std::cout;
		cout << "\n+---+---+---+---+---+---+---+---+\n";
		for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
			for (File file = FILE_A; file <= FILE_H; ++file) {
				cout << "| " << get_piece(Coord(rank, file)) << " ";
			}
			cout << "|\n+---+---+---+---+---+---+---+---+\n";
		}
		cout << "\n";
	}

private:
	/// <summary>
	/// 각각의 기물은 이름의 초성으로 표현되며, FEN 방식을 따른다.
	/// <para>
	///		백이 대문자, 흑이 소문자이다.
	/// </para>
	/// </summary>
	char board[64] = {};

	/// <summary>
	/// 좌표 하나를 받아서 해당 좌표가 체스판 위에 존재하는지를 확인한다.
	/// </summary>
	/// <param name="coord"> 확인을 원하는 좌표 </param>
	/// <returns> 좌표가 체스판 위에 있으면 참, 아니면 거짓 </returns>
	inline bool is_on_board(Coord coord) const {
		if (RANK_1 <= coord.rank && coord.rank <= RANK_8 &&
			FILE_A <= coord.file && coord.file <= FILE_H)
			return true;
		else
			return false;
	}

	/// <summary>
	/// 좌표 하나와 진영을 받아서 해당 위치에 말을 놓을 수 있는지를 판별하는 함수이다.
	/// <para>
	///		기물을 움직이기 이전에 그것이 가능한지를 판별한다.
	/// </para>
	/// </summary>
	/// <param name="dst_crd"> 확인을 원하는 좌표 </param>
	/// <param name="side"> 진영 </param>
	/// <returns> 좌표가 비여있거나 적의 기물이 있으면 참, 아니면 거짓 </returns>
	inline bool movable_piece_to(const Coord& dst_crd, Side side) const {
		if (get_side(dst_crd) != side)
			return true;
		else
			return false;
	}

	/// <summary>
	/// movable_to()가 true임을 전제로 기물의 움직임이 실현된 결과를 반환한다
	/// <para>
	///		위의 movable_to()가 true임을 전제로 사용한다.
	/// </para>
	/// </summary>
	/// <param name="src_crd"> 원래 기물의 위치 </param>
	/// <param name="dst_crd"> 목표 기물의 위치 </param>
	/// <returns> Board type의 객체 </returns>
	inline Board& move_piece_to(Coord src_crd, Coord dst_crd) const {
		Board potential_board = *this;
		potential_board.set_piece(dst_crd, potential_board.get_piece(src_crd));
		potential_board.set_piece(src_crd, ' ');
		return potential_board;
	}
	 
	/// <summary>
	/// 왕이 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 왕의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Board> movable_k(Coord src_crd, Side side) const {
		vector<Board> result;
		const int move_r[8] = { 1, 1, 1, 0, 0,-1,-1,-1 };
		const int move_f[8] = { 1, 0,-1, 1,-1, 1, 0,-1 };
		for (int i = 0; i < 8; ++i) {
			Coord dst_crd = Coord(
				src_crd.rank + move_r[i],
				src_crd.file + move_f[i]);
			if (is_on_board(dst_crd) && movable_piece_to(dst_crd, side)) {
					Board potential_board = move_piece_to(src_crd, dst_crd);
					result.push_back(potential_board);
				}
			}
		return result;
	}

	/// <summary>
	/// 퀸이 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 퀸의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Board>& movable_q(Coord src_crd, Side side) const {
		vector<Board> result;
		vector<Board> b_movable = movable_b(src_crd, side);
		vector<Board> r_movable = movable_r(src_crd, side);
		result.insert(result.end(), b_movable.begin(), b_movable.end());
		result.insert(result.end(), r_movable.begin(), r_movable.end());
		return result;
	}

	/// <summary>
	/// 룩이 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 룩의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Board> movable_r(Coord src_crd, Side side) const {
		vector<Board> result;
		for (Rank rank = src_crd.rank + 1; rank <= RANK_8; ++rank) {
			Coord dst_crd = Coord(rank, src_crd.file);
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (Rank rank = src_crd.rank - 1; rank >= RANK_1; --rank) {
			Coord dst_crd = Coord(rank, src_crd.file);
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (File file = src_crd.file + 1; file <= FILE_H; ++file) {
			Coord dst_crd = Coord(src_crd.rank, file);
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (File file = src_crd.file - 1; file >= FILE_A; --file) {
			Coord dst_crd = Coord(src_crd.rank, file);
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		return result;
	}

	/// <summary>
	/// 비숍이 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 비숍의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Board> movable_b(Coord src_crd, Side side) const {
		vector<Board> result;
		for (Coord dst_crd{ src_crd.rank + 1, src_crd.file + 1 }; is_on_board(dst_crd); ++dst_crd.rank, ++dst_crd.file) {
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (Coord dst_crd{ src_crd.rank + 1, src_crd.file - 1 }; is_on_board(dst_crd); ++dst_crd.rank, --dst_crd.file) {
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (Coord dst_crd{ src_crd.rank - 1, src_crd.file + 1 }; is_on_board(dst_crd); --dst_crd.rank, ++dst_crd.file) {
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (Coord dst_crd{ src_crd.rank - 1, src_crd.file - 1 }; is_on_board(dst_crd); --dst_crd.rank, --dst_crd.file) {
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		return result;
	}

	/// <summary>
	/// 나이트가 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 나이트의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Board> movable_n(Coord src_crd, Side side) const {
		vector<Board> result;
		const int move_r[8] = { 1, 1,-1,-1, 2, 2,-2,-2 };
		const int move_f[8] = { 2,-2, 2,-2, 1,-1, 1,-1 };
		for (int i = 0; i < 8; ++i) {
			Coord dst_crd = Coord(
				src_crd.rank + move_r[i],
				src_crd.file + move_f[i]);
			if (is_on_board(dst_crd) && movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
		}
		return result;
	}

	/// <summary>
	/// 폰이 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 폰의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Board> movable_p(Coord src_crd, Side side) const {
		vector<Board> result;
		if (side == WHITE) {
			Coord dst_crd = Coord(src_crd.rank + 1, src_crd.file);
			if (get_side(dst_crd) == EMPTY) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);

				if (src_crd.rank == RANK_2) {
					++dst_crd.rank;
					if (get_side(dst_crd) == EMPTY) {
						Board potential_board = move_piece_to(src_crd, dst_crd);
						result.push_back(potential_board);
					}
				}
			}

			dst_crd = Coord(src_crd.rank + 1, src_crd.file + 1);
			if (is_on_board(dst_crd) && get_side(dst_crd) == !side) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			dst_crd = Coord(src_crd.rank + 1, src_crd.file - 1);
			if (is_on_board(dst_crd) && get_side(dst_crd) == !side) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
		}
		else {
			Coord dst_crd = Coord(src_crd.rank - 1, src_crd.file);
			if (get_side(dst_crd) == EMPTY) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);

				if (src_crd.rank == RANK_7) {
					--dst_crd.rank;
					if (get_side(dst_crd) == EMPTY) {
						Board potential_board = move_piece_to(src_crd, dst_crd);
						result.push_back(potential_board);
					}
				}
			}

			dst_crd = Coord(src_crd.rank - 1, src_crd.file + 1);
			if (is_on_board(dst_crd) && get_side(dst_crd) == !side) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			dst_crd = Coord(src_crd.rank - 1, src_crd.file - 1);
			if (is_on_board(dst_crd) && get_side(dst_crd) == !side) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
		}
		return result;
	}

};

#endif // !BOARD_H_INCLUDED