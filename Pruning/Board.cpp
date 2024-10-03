#include <functional>
#include "Board.h"

//! ����׿� - print(), print_movable_cases() �Լ����� ���
#include <iostream>
#include <Windows.h>
#include <conio.h>


Board::Board() {
	for (int i = 0; i < 64; ++i)
		board[i] = ' ';
	turn = EMPTY;
	castling_K = false;
	castling_Q = false;
	castling_k = false;
	castling_q = false;
	en_passant = Coord();
	half_move = 0;
	full_move = 0;
}
Board::Board(string FEN) {
	*this = Board();

	int64_t src_idx = -1;
	int64_t dst_idx = -1;
	std::function<string()> split = [&]() {
		src_idx = dst_idx + 1;
		dst_idx = FEN.find(' ', src_idx);
		return FEN.substr(src_idx, dst_idx - src_idx);
		};

	string position_str = split();
	string turn_str = split();
	string castling_str = split();
	string en_passant_str = split();
	string half_move_str = split();
	string full_move_str = split();

	//�⹰�� ��ġ
	Rank rank = RANK_8;
	File file = FILE_A;
	for (char c : position_str) {
		if (c == ' ') {
			break;
		}
		else if (c == '/'); //c�� '/'�� ���� �����Ѵ�.
		else if (isalpha(c)) {
			set_piece(Coord(rank, file), c);
			++file;
		}
		else if (isdigit(c)) {
			file = (File)(file + (c - '0'));	//file += c - '0'
		}
		rank = (Rank)(rank - file / 8);			//rank += file / 8;
		file = (File)(file % 8);				//file %= 8;
	}

	//����
	switch (turn_str[0]) {
	case 'W': case 'w': turn = WHITE; break;
	case 'B': case 'b': turn = BLACK; break;
	default:
		break;
	}

	//ĳ���� ���� ����
	for (char c : castling_str) {
		switch (c) {
		case 'K': castling_K = true; break;
		case 'Q': castling_Q = true; break;
		case 'k': castling_k = true; break;
		case 'q': castling_q = true; break;
		default:
			break;
		}
	}

	//���Ļ� ������ ĭ
	if (en_passant_str != "-") {
		if (en_passant_str[0] >= 'A' &&
			en_passant_str[0] <= 'Z')
			en_passant_str[0] += 'a' - 'A';
		en_passant = Coord(
			RANK_1 + (en_passant_str[1] - '1'),
			FILE_A + (en_passant_str[0] - 'a')
		);
	}

	//��������
	if (half_move_str != "-")
		half_move = std::stoi(half_move_str);

	//Ǯ����
	if (full_move_str != "-")
		full_move = std::stoi(full_move_str);
}


string Board::to_fen() const {
	string fen;

	//�⹰�� ��ġ
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
	fen += ' ';

	//����
	switch (turn) {
	case WHITE: fen += 'w'; break;
	case BLACK: fen += 'b'; break;
	default:	fen += '-'; break;
	}
	fen += ' ';

	//ĳ���� ���� ����
	if (castling_K || castling_Q ||
		castling_k || castling_q) {
		if (castling_K) fen += 'K';
		if (castling_Q) fen += 'Q';
		if (castling_k) fen += 'k';
		if (castling_q) fen += 'q';
	}
	else {
		fen += '-';
	}
	fen += ' ';

	//���Ļ� ������ ĭ
	if (en_passant == Coord()) {
		fen += '-';
	}
	else {
		fen += 'a' + en_passant.file;
		fen += '1' + en_passant.rank;
	}
	fen += ' ';

	//��������
	fen += std::to_string(half_move);
	fen += ' ';

	//Ǯ����
	fen += std::to_string(full_move);

	return fen;
}
Board Board::make_moved_board(Move move) const {
	Board potential_board = *this;
	potential_board.apply_move(move);
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
			//���� ���� ������ ���� ������ ����� ���� ����ϴ� �Լ���
			//	�޸� �����Ͽ� ���� �� ���� add�� �����Ѵ�.
			switch (get_piece(src_crd)) {
			case 'K': case 'k': { add = movable_k(src_crd, side); break; }
			case 'Q': case 'q': { add = movable_q(src_crd, side); break; }
			case 'R': case 'r': { add = movable_r(src_crd, side); break; }
			case 'B': case 'b': { add = movable_b(src_crd, side); break; }
			case 'N': case 'n': { add = movable_n(src_crd, side); break; }
			case 'P': case 'p': { add = movable_p(src_crd, side); break; }
			}
			//add�� ��ҵ��� result�� �߰��ȴ�.
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
	//[0,64)�� [0,8), [8,16), [16,24), [24,32), [32,40), [40,48), [48,56), [56,64)�� 8���� �������� ������,
	//�� �������� ������� �׾Ƽ� 8*8�� '2���� �迭'�� ������ �� �ִ�.
	//�̶� Rank�� File�� ���� �迭�� ��� ���� �ǰ�,
	//������ ���̰� 8�̱� ������ rank�� file���� 1���� �迭���� 8*rank+file ��° ��ġ�� ���� ���̴�.
	board[8 * coord.rank + coord.file] = piece;
}
char Board::get_piece(Coord coord) const {
	//64ĭ�� ��ũ�� 8�� ���.1��ũ �ö� ������ 64ĭ Array���� index�� 8 ����.(E.g | e���� 4��ũ�� 5�� 4�� ( 5+4*8 ))
	return board[8 * coord.rank + coord.file];
}
Side Board::get_side(Coord coord) const {
	const char piece = get_piece(coord);
	if ('A' <= piece && piece <= 'Z') return WHITE;
	if ('a' <= piece && piece <= 'z') return BLACK;
	return EMPTY;
}


inline bool Board::movable_piece(const Coord& dst_crd, Side side) const {
	if (get_side(dst_crd) != side)
		return true;
	else
		return false;
}
inline void Board::move_piece_to(Coord src_crd, Coord dst_crd) {
	const char move_piece = get_piece(src_crd);
	set_piece(dst_crd, move_piece);
	set_piece(src_crd, ' ');
}
void Board::apply_move(Move move) {
	const char move_piece = get_piece(move.src);
#pragma region before move
	//���������� ����
	if (get_side(move.dst) == EMPTY)
		half_move++;
	else
		half_move = 0;
	//Ǯ������ ����
	full_move += (turn == BLACK) ? 1 : 0;
#pragma endregion
#pragma region move
	//������� ������
	set_piece(move.dst, move_piece);
	set_piece(move.src, ' ');
	//ĳ����
	//	�̹� movable_k()���� ĳ���� ���� ���θ� �Ǵ�������, ���� Ȯ������ �ʴ´�
	//	ŷ�� �̹� �������� ������ �踸 �߰������� �����δ�.
	if (move_piece == 'K') {
		//�� ŷ���̵�
		if (castling_K && move.dst == Coord(RANK_1, FILE_G)) {
			move_piece_to(
				Coord(RANK_1, FILE_H),
				Coord(RANK_1, FILE_F));
			castling_K = false;
			castling_Q = false;
		}
		//�� �����̵�
		if (castling_Q && move.dst == Coord(RANK_1, FILE_C)) {
			move_piece_to(
				Coord(RANK_1, FILE_A),
				Coord(RANK_1, FILE_D));
			castling_K = false;
			castling_Q = false;
		}
	}
	if (move_piece == 'k') {
		//�� ŷ���̵�
		if (castling_k && move.dst == Coord(RANK_8, FILE_G)) {
			move_piece_to(
				Coord(RANK_8, FILE_H),
				Coord(RANK_8, FILE_F));
			castling_k = false;
			castling_q = false;
		}
		//�� �����̵�
		if (castling_q && move.dst == Coord(RANK_8, FILE_C)) {
			move_piece_to(
				Coord(RANK_8, FILE_A),
				Coord(RANK_8, FILE_D));
			castling_k = false;
			castling_q = false;
		}
	}
#pragma endregion
#pragma region after move

	//ĳ���� ������ ����
	switch (move_piece) {
	case 'K':
		castling_K = false;
		castling_Q = false;
		break;
	case 'R':
		if (move.src == Coord(RANK_1, FILE_H)) castling_K = false; //ŷ���̵�
		if (move.src == Coord(RANK_1, FILE_A)) castling_Q = false; //�����̵�
		break;

	case 'k':
		castling_k = false;
		castling_q = false;
		break;
	case 'r':
		if (move.src == Coord(RANK_8, FILE_H)) castling_k = false; //ŷ���̵�
		if (move.src == Coord(RANK_8, FILE_A)) castling_q = false; //�����̵�
		break;

	default:
		break;
	}

	//���Ļ� ��ġ�� ����
	en_passant = Coord();
	if (move_piece == 'P' &&
		move.src.rank == RANK_2 &&
		move.dst.rank == RANK_4)
		en_passant = Coord(RANK_3, move.src.file);
	if (move_piece == 'p' &&
		move.src.rank == RANK_7 &&
		move.dst.rank == RANK_5)
		en_passant = Coord(RANK_3, move.src.file);

	//������ ��ȯ
	switch (turn) {
	case WHITE: turn = BLACK; break;
	case BLACK: turn = WHITE; break;
	default:
		throw;
	}
#pragma endregion
}


//���߿� movable() ���� ���� ���ư����� �ٽ� ¥ ����.
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


//ĳ���� ����
inline vector<Move> Board::movable_k(Coord src_crd, Side side) const {
	vector<Move> result;
	constexpr int move_r[8] = { 1, 1, 1, 0, 0,-1,-1,-1 };
	constexpr int move_f[8] = { -1, 0, 1,-1, 1,-1, 0, 1 };
	for (int i = 0; i < 8; ++i) {
		Coord dst_crd = Coord(
			src_crd.rank + move_r[i],
			src_crd.file + move_f[i]);
		if (dst_crd.on_board() && movable_piece(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
		}
	}

	return result;

	//ĳ������ ������ ���, ĳ������ ������ ��츦 �߰��Ѵ�.
	//������ ����, ĳ������ ŷ �Ӹ��� �ƴ϶� ����� �����̴� Ư���� ��Ģ�̶�� ���̴�.
	//���� �̶� ĳ������ �Ϲ������� 1ĭ���� �����̴� ŷ�� 2ĭ�� �����δ�.
	//	����, ĳ������ ŷ�� 2ĭ �����̴� ���� �״�� �־� ������ �ȴ�!
	if (side == WHITE) {
		if (castling_K) {
			//���� �޼� Ȯ�� - 3�� ������ üũ �Լ��� ���� ���ϸ� �� �� ����.(�׷��� �Ϸ��� �翬�� üũ �Լ��� �ٽ� ������ �ϰ�����)
			result.push_back(Move(
				Coord(RANK_1, FILE_E),
				Coord(RANK_1, FILE_G)));
		}
		if (castling_Q) {
			//���� �޼� Ȯ��
			result.push_back(Move(
				Coord(RANK_1, FILE_E),
				Coord(RANK_1, FILE_C)));
		}
	}
	else {
		if (castling_k) {
			//���� �޼� Ȯ��
			result.push_back(Move(
				Coord(RANK_8, FILE_E),
				Coord(RANK_8, FILE_G)));
		}
		if (castling_q) {
			//���� �޼� Ȯ��
			result.push_back(Move(
				Coord(RANK_8, FILE_E),
				Coord(RANK_8, FILE_C)));
		}
	}
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
		if (movable_piece(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (File file = src_crd.file + 1; file <= FILE_H; ++file) {
		Coord dst_crd = Coord(src_crd.rank, file);
		if (movable_piece(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Rank rank = src_crd.rank - 1; rank >= RANK_1; --rank) {
		Coord dst_crd = Coord(rank, src_crd.file);
		if (movable_piece(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (File file = src_crd.file - 1; file >= FILE_A; --file) {
		Coord dst_crd = Coord(src_crd.rank, file);
		if (movable_piece(dst_crd, side)) {
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
	for (Coord dst_crd{ src_crd.rank + 1, src_crd.file - 1 }; dst_crd.on_board(); ++dst_crd.rank, --dst_crd.file) {
		if (movable_piece(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank + 1, src_crd.file + 1 }; dst_crd.on_board(); ++dst_crd.rank, ++dst_crd.file) {
		if (movable_piece(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank - 1, src_crd.file - 1 }; dst_crd.on_board(); --dst_crd.rank, --dst_crd.file) {
		if (movable_piece(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank - 1, src_crd.file + 1 }; dst_crd.on_board(); --dst_crd.rank, ++dst_crd.file) {
		if (movable_piece(dst_crd, side)) {
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
		if (dst_crd.on_board() && movable_piece(dst_crd, side)) {
			result.push_back(Move(src_crd, dst_crd));
		}
	}
	return result;
}
inline vector<Move> Board::movable_p(Coord src_crd, Side side) const {
	vector<Move> result;
	if (side == WHITE) {
		Coord dst_crd = Coord(src_crd.rank + 1, src_crd.file);
		if (dst_crd.on_board() && get_side(dst_crd) == EMPTY) {
			result.push_back(Move(src_crd, dst_crd));

			if (src_crd.rank == RANK_2) {
				++dst_crd.rank;
				if (get_side(dst_crd) == EMPTY)
					result.push_back(Move(src_crd, dst_crd));
			}
		}

		dst_crd = Coord(src_crd.rank + 1, src_crd.file + 1);
		if (dst_crd.on_board() && get_side(dst_crd) == !side ||
			dst_crd == en_passant)
			result.push_back(Move(src_crd, dst_crd));

		dst_crd = Coord(src_crd.rank + 1, src_crd.file - 1);
		if (dst_crd.on_board() && get_side(dst_crd) == !side ||
			dst_crd == en_passant)
			result.push_back(Move(src_crd, dst_crd));
	}
	else {
		Coord dst_crd = Coord(src_crd.rank - 1, src_crd.file);
		if (dst_crd.on_board() && get_side(dst_crd) == EMPTY) {
			result.push_back(Move(src_crd, dst_crd));

			if (src_crd.rank == RANK_7) {
				--dst_crd.rank;
				if (get_side(dst_crd) == EMPTY)
					result.push_back(Move(src_crd, dst_crd));
			}
		}

		dst_crd = Coord(src_crd.rank - 1, src_crd.file + 1);
		if (dst_crd.on_board() && get_side(dst_crd) == !side ||
			dst_crd == en_passant)
			result.push_back(Move(src_crd, dst_crd));

		dst_crd = Coord(src_crd.rank - 1, src_crd.file - 1);
		if (dst_crd.on_board() && get_side(dst_crd) == !side ||
			dst_crd == en_passant)
			result.push_back(Move(src_crd, dst_crd));
	}
	return result;
}
