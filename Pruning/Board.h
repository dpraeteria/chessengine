#pragma once
#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

//! 디버그용
#include <iostream>

#include <string>
#include <vector>
#include <cctype>
#include "Coord.h"
using std::string;
using std::vector;

enum Side : char { WHITE, BLACK, EMPTY };

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
	
	void set_piece(Coord coord, char piece) {
		board[8 * coord.rank + coord.file] = piece;
	}
	char get_piece(Coord coord) const {
		return board[8 * coord.rank + coord.file];
	}
	Side get_side(Coord coord) const {
		if ('A' <= get_piece(coord) &&
			'Z' >= get_piece(coord))
			return WHITE;
		if ('a' <= get_piece(coord) &&
			'z' >= get_piece(coord))
			return BLACK;
		return EMPTY;
	}

	//일단은 Board로 만들고 있으나, 추후에는 FEN와 호환되도록 할 것이다.
	vector<Board> movable_boards(Side is_white_side) const {
		vector<Board> result;

		for (Rank rank = RANK_1; rank <= RANK_8; ++rank) {
			for (File file = FILE_A; file <= FILE_H; ++file) {
				Coord src_crd = Coord(rank, file);

				if (get_side(src_crd) == is_white_side);
				else
					continue;

				vector<Board> add;
				//놓인 말의 종류에 따라 움질임 경우의 수를 계산하는 함수를
				//	달리 구현하여 실행 및 변수 add에 대입한다.
				switch (get_piece(src_crd)) {
				case 'K': { add = movable_k(src_crd, is_white_side); break; }
				case 'Q': { add = movable_q(src_crd, is_white_side); break; }
				case 'R': { add = movable_r(src_crd, is_white_side); break; }
				case 'B': { add = movable_b(src_crd, is_white_side); break; }
				case 'N': { add = movable_n(src_crd, is_white_side); break; }
				case 'P': { add = movable_p(src_crd, is_white_side); break; }
				}
				//add의 요소들은 result에 추가된다.
				result.insert(result.end(),
					add.begin(), add.end());
			}
		}
		return result;
	}

	//! 디버그용
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
	//기물의 흑백은 알파벳의 대/소문자로 구분하며,
	//이는 FEN 방식과 마찬가지로 흑이 소문자, 백이 대문자이다.
	char board[64] = {};

	inline bool is_on_board(Coord coord) const {
		if (RANK_1 <= coord.rank && coord.rank <= RANK_8 &&
			FILE_A <= coord.file && coord.file <= FILE_H)
			return true;
		else
			return false;
	}
	//dst_crd의 위치에 기물이 위치할 수 있는지 판별하는 함수
	//is_white_side와 같은 진영의 말이 존재하지 않는 경우 true를 반환한다.
	inline bool movable_piece_to(const Coord& dst_crd, Side side) const {
		if (get_side(dst_crd) != side)
			return true;
		else
			return false;
	}
	//위의 movable_to()가 true임을 전제로 사용한다.
	//src_crd의 기물을 dst_crd의 위치에 이동시킨 보드를 반환한다.
	inline Board& move_piece_to(const Coord& src_crd, const Coord& dst_crd) const {
		Board potential_board = *this;
		potential_board.set_piece(dst_crd, potential_board.get_piece(src_crd));
		potential_board.set_piece(src_crd, ' ');
		return potential_board;
	}
	 
	inline vector<Board> movable_k(Coord src_crd, Side side) const {
		vector<Board> result;
		//이렇게 -1이 버젓이 존재하는 것을 명백히 올바르지 않은 사용이긴 하다.
		const Coord move_rf[] = {
			Coord(-1, -1), Coord(-1,  0), Coord(-1,  1),
			Coord( 0, -1),                Coord( 0,  1),
			Coord( 1, -1), Coord( 1,  0), Coord( 1,  1),
		};
		for (Coord move : move_rf) {
			Coord dst_crd = Coord(
				src_crd.rank + move.rank,
				src_crd.file + move.file);
			if (is_on_board(dst_crd) && movable_piece_to(dst_crd, side)) {
					Board potential_board = move_piece_to(src_crd, dst_crd);
					result.push_back(potential_board);
				}
			}
		return result;
	}
	inline vector<Board> movable_q(Coord src_crd, Side side) const {
		vector<Board> result;
		vector<Board> b_movable = movable_b(src_crd, side);
		vector<Board> r_movable = movable_r(src_crd, side);
		result.insert(result.end(), b_movable.begin(), b_movable.end());
		result.insert(result.end(), r_movable.begin(), r_movable.end());
		return result;
	}
	inline vector<Board> movable_r(Coord src_crd, Side side) const {
		vector<Board> result;
		for (Rank rank = Rank(src_crd.rank + 1); rank <= RANK_8; ++rank) {
			Coord dst_crd = Coord(rank, src_crd.file);
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (Rank rank = Rank(src_crd.rank - 1); rank >= RANK_1; --rank) {
			Coord dst_crd = Coord(rank, src_crd.file);
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (File file = File(src_crd.rank + 1); file <= FILE_H; ++file) {
			Coord dst_crd = Coord(src_crd.rank, file);
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (File file = File(src_crd.rank - 1); file >= FILE_A; --file) {
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
	inline vector<Board> movable_b(Coord src_crd, Side side) const {
		vector<Board> result;
		for (Coord dst_crd = Coord(src_crd.rank + 1, src_crd.file + 1); is_on_board(dst_crd); ++dst_crd.rank, ++dst_crd.file) {
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (Coord dst_crd = Coord(src_crd.rank + 1, src_crd.file - 1); is_on_board(dst_crd); ++dst_crd.rank, --dst_crd.file) {
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (Coord dst_crd = Coord(src_crd.rank - 1, src_crd.file + 1); is_on_board(dst_crd); --dst_crd.rank, ++dst_crd.file) {
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		for (Coord dst_crd = Coord(src_crd.rank - 1, src_crd.file - 1); is_on_board(dst_crd); --dst_crd.rank, --dst_crd.file) {
			if (movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
			else
				break;
		}
		return result;
	}
	inline vector<Board> movable_n(Coord src_crd, Side side) const {
		vector<Board> result;
		const Coord move_rf[] = {
			Coord( 1,  2),
			Coord( 1, -2),
			Coord(-1,  2),
			Coord(-1, -2),

			Coord( 2,  1),
			Coord( 2, -1),
			Coord(-2,  1),
			Coord(-2, -1),
		};
		for (Coord move : move_rf) {
			Coord dst_crd = Coord(
				src_crd.rank + move.rank,
				src_crd.file + move.file);
			if (is_on_board(dst_crd) && movable_piece_to(dst_crd, side)) {
				Board potential_board = move_piece_to(src_crd, dst_crd);
				result.push_back(potential_board);
			}
		}
		return result;
	}
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