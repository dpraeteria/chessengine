#include "Board.h"

string Board::to_fen() const { //분해는 조립의 역순. 구현에는 생성자를 참고하자.
	string fen;
	int empty_cnt = 0;
	for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
		for (File file = FILE_A; file <= FILE_H; ++file) {
			const char piece = get_piece(Coord(rank, file));
			if (piece == 'E' || piece == 'e' || piece == ' ') {
				empty_cnt++;
			}
			else {
				if (empty_cnt != 0) {
					fen += std::to_string(empty_cnt);
					empty_cnt = 0;
				}
				fen += piece;
			}
		}
		if (empty_cnt != 0) {
			fen += std::to_string(empty_cnt);
			empty_cnt = 0;
		}
		fen += '/';
	}
	fen.pop_back();
	return fen;
}
Board Board::apply_move(Move move) const {
	Board potential_board = *this;
	//앙파상 가능 칸 제거
	for (File file = FILE_A; file <= FILE_H; ++file) {
		Coord w_crd = Coord(RANK_3, file);
		Coord b_crd = Coord(RANK_6, file);
		if (potential_board.get_piece(w_crd) == 'E')
			potential_board.set_piece(w_crd, ' ');
		if (potential_board.get_piece(b_crd) == 'e')
			potential_board.set_piece(b_crd, ' ');
	}
	potential_board.set_piece(move.dst, potential_board.get_piece(move.src));
	potential_board.set_piece(move.src, ' ');
	//앙파상 가능 칸 생성
	if (potential_board.get_piece(move.dst) == 'P' &&
		move.src.rank == RANK_2 &&
		move.dst.rank == RANK_4)
		potential_board.set_piece(Coord(RANK_3, move.src.file), 'E');
	if (potential_board.get_piece(move.dst) == 'p' &&
		move.src.rank == RANK_7 &&
		move.dst.rank == RANK_5)
		potential_board.set_piece(Coord(RANK_6, move.src.file), 'e');
	return potential_board;
}
vector<Move> Board::movable_cases(Side side) const {
	vector<Move> result;

	for (Rank rank = RANK_1; rank <= RANK_8; ++rank) {
		for (File file = FILE_A; file <= FILE_H; ++file) {
			Coord src_crd = Coord(rank, file);

			if (get_side(src_crd) == side);
			else
				continue;

			vector<Move> add;
			//놓인 말의 종류에 따라 움질임 경우의 수를 계산하는 함수를
			//	달리 구현하여 실행 및 변수 add에 대입한다.
			switch (get_piece(src_crd)) {
			case 'K': case 'k': { add = movable_k(src_crd, side); break; }
			case 'Q': case 'q': { add = movable_q(src_crd, side); break; }
			case 'R': case 'r': { add = movable_r(src_crd, side); break; }
			case 'B': case 'b': { add = movable_b(src_crd, side); break; }
			case 'N': case 'n': { add = movable_n(src_crd, side); break; }
			case 'P': case 'p': { add = movable_p(src_crd, side); break; }
			}
			//add의 요소들은 result에 추가된다.
			result.insert(result.end(),
				add.begin(), add.end());
		}
	}
	return result;
}


void Board::print() const {
	using std::cout;
	cout << "+---+---+---+---+---+---+---+---+\n";
	for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
		for (File file = FILE_A; file <= FILE_H; ++file) {
			cout << "| " << get_piece(Coord(rank, file)) << " ";
		}
		cout << "|\n+---+---+---+---+---+---+---+---+\n";
	}
	cout << "\n";
}
void Board::print_movable_cases(Side side) const {
	using std::cout;
	const std::function<void(int)> set_color = [](int color) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		};

	vector<Move> moves = movable_cases(side);
	for (Move move : moves) {
		system("cls");
		cout << "+---+---+---+---+---+---+---+---+\n";
		for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
			for (File file = FILE_A; file <= FILE_H; ++file) {
				cout << "|";
				if (move.src.rank == rank &&
					move.src.file == file ||
					move.dst.rank == rank &&
					move.dst.file == file)
					set_color(0x30);
				else if (get_side(Coord(rank, file)) == WHITE) set_color(0xF0);
				else if (get_side(Coord(rank, file)) == BLACK) set_color(0x80);
				cout << " " << get_piece(Coord(rank, file)) << " ";
				set_color(0x07);
			}
			cout << "|\n+---+---+---+---+---+---+---+---+\n";
		}
		cout << "\n";

		while (true) {
			char order = _getch();
			if (order == ' ')
				break;
		}
	}
}


void Board::set_piece(Coord coord, char piece) {
	//[0,64)를 [0,8), [8,16), [16,24), [24,32), [32,40), [40,48), [48,56), [56,64)의 8개의 구간으로 나누고,
	//이 구간들을 순서대로 쌓아서 8*8의 '2차원 배열'로 생각할 수 있다.
	//이때 Rank와 File은 각각 배열의 행과 열이 되고,
	//구간의 길이가 8이기 때문에 rank행 file열은 1차원 배열에서 8*rank+file 번째 위치와 같은 것이다.
	board[8 * coord.rank + coord.file] = piece;
}
char Board::get_piece(Coord coord) const {
	//64칸중 랭크는 8의 배수.1랭크 올라갈 때마다 64칸 Array에서 index는 8 증가.(E.g | e파일 4랭크는 5행 4열 ( 5+4*8 ))
	return board[8 * coord.rank + coord.file];
}


Side Board::get_side(Coord coord) const {
	if ('A' <= get_piece(coord) &&
		'Z' >= get_piece(coord))
		return WHITE;
	if ('a' <= get_piece(coord) &&
		'z' >= get_piece(coord))
		return BLACK;
	return EMPTY;
}
inline bool Board::is_on_board(Coord coord) const {
	if (RANK_1 <= coord.rank && coord.rank <= RANK_8 &&
		FILE_A <= coord.file && coord.file <= FILE_H)
		return true;
	else
		return false;
}
inline bool Board::movable_piece_to(const Coord& dst_crd, Side side) const {
	if (get_side(dst_crd) != side)
		return true;
	else
		return false;
}
inline Board& Board::move_piece_to(Coord src_crd, Coord dst_crd) const {
	Board potential_board = *this;
	potential_board.set_piece(dst_crd, potential_board.get_piece(src_crd));
	potential_board.set_piece(src_crd, ' ');
	return potential_board;
}


inline Side Board::check() const {
	Coord w_king;
	Coord b_king;
	for (Rank rank = RANK_1; rank <= RANK_8; ++rank) {
		for (File file = FILE_A; file <= FILE_H; ++file) {
			Coord coord = Coord(rank, file);
			if (get_piece(coord) == 'K') w_king = coord;
			if (get_piece(coord) == 'k') b_king = coord;
		}
	}

	using std::find;
	bool w_checked = false;
	bool b_checked = false;
	vector<Move> w_cases = movable_cases(WHITE);
	vector<Move> b_cases = movable_cases(BLACK);
	for (Move w_move : w_cases) if (w_move.dst == b_king) { b_checked = true; break; }
	for (Move b_move : b_cases) if (b_move.dst == w_king) { w_checked = true; break; }

	if (w_checked && b_checked)
		return GREY;
	else if (w_checked)	return WHITE;
	else if (b_checked)	return BLACK;
	else				return EMPTY;
}


inline vector<Move> Board::movable_k(Coord src_crd, Side side) const {
	vector<Move> result;
	constexpr int move_r[8] = { 1, 1, 1, 0, 0,-1,-1,-1 };
	constexpr int move_f[8] = { -1, 0, 1,-1, 1,-1, 0, 1 };
	for (int i = 0; i < 8; ++i) {
		Coord dst_crd = Coord(
			src_crd.rank + move_r[i],
			src_crd.file + move_f[i]);
		if (is_on_board(dst_crd) && movable_piece_to(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
		}
	}
	return result;
}
inline vector<Move> Board::movable_q(Coord src_crd, Side side) const {
	vector<Move> result;
	vector<Move> b_movable = movable_b(src_crd, side);
	vector<Move> r_movable = movable_r(src_crd, side);
	result.insert(result.end(), b_movable.begin(), b_movable.end());
	result.insert(result.end(), r_movable.begin(), r_movable.end());
	return result;
}
inline vector<Move> Board::movable_r(Coord src_crd, Side side) const {
	vector<Move> result;
	for (Rank rank = src_crd.rank + 1; rank <= RANK_8; ++rank) {
		Coord dst_crd = Coord(rank, src_crd.file);
		if (movable_piece_to(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (File file = src_crd.file + 1; file <= FILE_H; ++file) {
		Coord dst_crd = Coord(src_crd.rank, file);
		if (movable_piece_to(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Rank rank = src_crd.rank - 1; rank >= RANK_1; --rank) {
		Coord dst_crd = Coord(rank, src_crd.file);
		if (movable_piece_to(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (File file = src_crd.file - 1; file >= FILE_A; --file) {
		Coord dst_crd = Coord(src_crd.rank, file);
		if (movable_piece_to(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	return result;
}
inline vector<Move> Board::movable_b(Coord src_crd, Side side) const {
	vector<Move> result;
	for (Coord dst_crd{ src_crd.rank + 1, src_crd.file - 1 }; is_on_board(dst_crd); ++dst_crd.rank, --dst_crd.file) {
		if (movable_piece_to(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank + 1, src_crd.file + 1 }; is_on_board(dst_crd); ++dst_crd.rank, ++dst_crd.file) {
		if (movable_piece_to(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank - 1, src_crd.file - 1 }; is_on_board(dst_crd); --dst_crd.rank, --dst_crd.file) {
		if (movable_piece_to(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank - 1, src_crd.file + 1 }; is_on_board(dst_crd); --dst_crd.rank, ++dst_crd.file) {
		if (movable_piece_to(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	return result;
}
inline vector<Move> Board::movable_n(Coord src_crd, Side side) const {
	vector<Move> result;
	constexpr int move_r[8] = { 2, 2, 1,-1,-2,-2,-1, 1, };
	constexpr int move_f[8] = { -1, 1, 2, 2, 1,-1,-2,-2, };
	for (int i = 0; i < 8; ++i) {
		Coord dst_crd = Coord(
			src_crd.rank + move_r[i],
			src_crd.file + move_f[i]);
		if (is_on_board(dst_crd) && movable_piece_to(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
		}
	}
	return result;
}
inline vector<Move> Board::movable_p(Coord src_crd, Side side) const {
	vector<Move> result;
	if (side == WHITE) {
		Coord dst_crd = Coord(src_crd.rank + 1, src_crd.file);
		if (is_on_board(dst_crd) && get_side(dst_crd) == EMPTY) {
			result.push_back(Move(src_crd, dst_crd));

			if (src_crd.rank == RANK_2) {
				++dst_crd.rank;
				if (get_side(dst_crd) == EMPTY)
					result.push_back(Move(src_crd, dst_crd));
			}
		}

		dst_crd = Coord(src_crd.rank + 1, src_crd.file + 1);
		if (is_on_board(dst_crd) && get_side(dst_crd) == !side)
			result.push_back(Move(src_crd, dst_crd));
		dst_crd = Coord(src_crd.rank + 1, src_crd.file - 1);
		if (is_on_board(dst_crd) && get_side(dst_crd) == !side)
			result.push_back(Move(src_crd, dst_crd));
	}
	else {
		Coord dst_crd = Coord(src_crd.rank - 1, src_crd.file);
		if (is_on_board(dst_crd) && get_side(dst_crd) == EMPTY) {
			result.push_back(Move(src_crd, dst_crd));

			if (src_crd.rank == RANK_7) {
				--dst_crd.rank;
				if (get_side(dst_crd) == EMPTY)
					result.push_back(Move(src_crd, dst_crd));
			}
		}

		dst_crd = Coord(src_crd.rank - 1, src_crd.file + 1);
		if (is_on_board(dst_crd) && get_side(dst_crd) == !side)
			result.push_back(Move(src_crd, dst_crd));
		dst_crd = Coord(src_crd.rank - 1, src_crd.file - 1);
		if (is_on_board(dst_crd) && get_side(dst_crd) == !side)
			result.push_back(Move(src_crd, dst_crd));
	}
	return result;
}
