#include <functional>
#include "Board.h"

//! ����׿� - print(), print_movable_cases() �Լ����� ���
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include "pruning_tree.h"


Board::Board() {
	for (int idx = WK; idx <= BP; ++idx)
		_exist[idx] = 0;
	turn = Empty;
	castling_K = false;
	castling_Q = false;
	castling_k = false;
	castling_q = false;
	en_passant = Coord();
	half_move = 0;
	full_move = 0;
}
Board::Board(const string& FEN) {
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
			PieceType p_type = Non;
			switch (c) {
			case 'K': p_type = WK; break;
			case 'Q': p_type = WQ; break;
			case 'R': p_type = WR; break;
			case 'B': p_type = WB; break;
			case 'N': p_type = WN; break;
			case 'P': p_type = WP; break;
			case 'k': p_type = BK; break;
			case 'q': p_type = BQ; break;
			case 'r': p_type = BR; break;
			case 'b': p_type = BB; break;
			case 'n': p_type = BN; break;
			case 'p': p_type = BP; break;
			}
			set_on_piece(p_type, Coord(rank, file));
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
	case 'W': case 'w': turn = White; break;
	case 'B': case 'b': turn = Black; break;
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
			if (piece != ' ') {
				if (empty_cnt != 0) {
					fen += std::to_string(empty_cnt);
					empty_cnt = 0;
				}
				fen += piece;
			}
			else {
				empty_cnt++;
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
	case White: fen += 'w'; break;
	case Black: fen += 'b'; break;
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


inline void Board::set_on_piece(PieceType piece, const Coord& coord) {
	int crd = 8 * coord.rank + coord.file;
	for (int idx = WK; idx <= BP; ++idx)
		_exist[idx] &= ~(1ull << crd);
	_exist[piece] |= (1ull << crd);
}
inline void Board::del_on_coord(PieceType piece, const Coord& coord) {
	int crd = 8 * coord.rank + coord.file;
	_exist[piece] &= ~(1ull << crd);
}
inline bool Board::get_on_piece(PieceType piece, const Coord& coord) const {
	int crd = 8 * coord.rank + coord.file;
	return _exist[piece] & (1ull << crd);
}
inline char Board::get_piece(const Coord& coord) const {
	PieceType p_type = Non;
	for (int idx = WK; idx <= BP; ++idx)
		if (get_on_piece(PieceType(idx), coord))
			p_type = PieceType(idx);

	switch (p_type) {
	case WK: return 'K';
	case WQ: return 'Q';
	case WR: return 'R';
	case WB: return 'B';
	case WN: return 'N';
	case WP: return 'P';

	case BK: return 'k';
	case BQ: return 'q';
	case BR: return 'r';
	case BB: return 'b';
	case BN: return 'n';
	case BP: return 'p';

	case Non:
	default:
		return ' ';
	}
}
inline Side Board::get_side(const Coord& coord) const {
	int crd = 8 * coord.rank + coord.file;
	for (int piece = WK; piece <= WP; ++piece) if (_exist[piece] & (1ull << crd)) return White;
	for (int piece = BK; piece <= BP; ++piece) if (_exist[piece] & (1ull << crd)) return Black;
	return Empty;
}


inline bool Board::check(const Coord& k_crd, Side side) const {
	for (Move move : __movable_k(k_crd, side))
		if (side == White && get_on_piece(BK, move.dst) ||
			side == Black && get_on_piece(WK, move.dst))
			return true;

	for (Move move : movable_r(k_crd, side))
		if (side == White && (get_on_piece(BR, move.dst) || get_on_piece(BQ, move.dst)) ||
			side == Black && (get_on_piece(WR, move.dst) || get_on_piece(WQ, move.dst)))
			return true;

	for (Move move : movable_b(k_crd, side))
		if (side == White && (get_on_piece(BB, move.dst) || get_on_piece(BQ, move.dst)) ||
			side == Black && (get_on_piece(WB, move.dst) || get_on_piece(WQ, move.dst)))
			return true;

	for (Move move : movable_n(k_crd, side))
		if (side == White && get_on_piece(BN, move.dst) ||
			side == Black && get_on_piece(WN, move.dst))
			return true;

	for (Move move : movable_p(k_crd, side))
		if (side == White && get_on_piece(BP, move.dst) ||
			side == Black && get_on_piece(WP, move.dst))
			return true;

	return false;
}
inline Side Board::check() const {
	Coord w_king;
	Coord b_king;
	for (Rank rank = RANK_1; rank <= RANK_8; ++rank) {
		for (File file = FILE_A; file <= FILE_H; ++file) {
			Coord coord = Coord(rank, file);
			if (get_on_piece(WK, coord)) w_king = coord;
			if (get_on_piece(BK, coord)) b_king = coord;
		}
	}

	bool w_checked = check(w_king, White);
	bool b_checked = check(b_king, Black);

	if (w_checked && b_checked)
		return Grey;
	else if (w_checked)	return White;
	else if (b_checked)	return Black;
	else				return Empty;
}
//�� �� ����ȭ�� ���·� ������ �ϳ�...
inline bool Board::checkmate() const {
	vector<Move> moves = movable_cases();
	for (Move move : moves) {
		Board new_board = make_moved_board(move);
		Side check_side = new_board.check();
		if (check_side == turn && check_side == Grey); //���� üũ�� ���� ��� �Ǵ°�...
		else
			return true;
	}
	return false;
}


inline bool Board::movable_piece(const Coord& dst_crd) const {
	if (get_side(dst_crd) != turn)
		return true;
	else
		return false;
}
inline void Board::move_piece_to(const Coord& src_crd, const Coord& dst_crd) {
	for (int idx = WK; idx <= BP; ++idx) {
		PieceType p_type = PieceType(idx);
		if (get_on_piece(p_type, src_crd)) {
			set_on_piece(p_type, dst_crd);
			del_on_coord(p_type, src_crd);
			break;
		}
	}
}
inline void Board::apply_move(const Move& move) {
#pragma region before move
	//���������� ����
	if (get_side(move.dst) == Empty)
		half_move++;
	else
		half_move = 0;
	//Ǯ������ ����
	full_move += (turn == Black) ? 1 : 0;
#pragma endregion
#pragma region move
	//������� ������
	move_piece_to(move.src, move.dst);
	//ĳ����
	//	�̹� movable_k()���� ĳ���� ���� ���θ� �Ǵ�������, ���� Ȯ������ �ʴ´�
	//	ŷ�� �̹� �������� ������ �踸 �߰������� �����δ�.
	if (get_on_piece(WK, move.dst)) {
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
	else if (get_on_piece(BK, move.dst)) {
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
	//���Ļ� ����
	if (en_passant == move.dst && (
		get_on_piece(WP, move.dst) ||
		get_on_piece(BP, move.dst))) {
		if (en_passant.rank == RANK_3)
			set_on_piece(Non, Coord(RANK_4, en_passant.file));
		if (en_passant.rank == RANK_6)
			set_on_piece(Non, Coord(RANK_5, en_passant.file));
	}
#pragma endregion
#pragma region after move

	//ĳ���� ������ ����
	if (get_on_piece(WK, move.dst)) {
		castling_K = false;
		castling_Q = false;
	}
	/*else*/ if (get_on_piece(WR, move.dst)) {
		if (move.src == Coord(RANK_1, FILE_H)) castling_K = false; //ŷ���̵�
		if (move.src == Coord(RANK_1, FILE_A)) castling_Q = false; //�����̵�
	}
	/*else*/ if (get_on_piece(BK, move.dst)) {
		castling_k = false;
		castling_q = false;
	}
	/*else*/ if (get_on_piece(BR, move.dst)) {
		if (move.src == Coord(RANK_8, FILE_H)) castling_k = false; //ŷ���̵�
		if (move.src == Coord(RANK_8, FILE_A)) castling_q = false; //�����̵�
	}

	//���Ļ� ��ġ�� ����
	en_passant = Coord();
	if (get_on_piece(WP, move.dst) &&
		move.src.rank == RANK_2 &&
		move.dst.rank == RANK_4)
		en_passant = Coord(RANK_3, move.src.file);
	/*else*/ if (get_on_piece(BP, move.dst) &&
		move.src.rank == RANK_7 &&
		move.dst.rank == RANK_5)
		en_passant = Coord(RANK_6, move.src.file);

	//������ ��ȯ
	switch (turn) {
	case White: turn = Black; break;
	case Black: turn = White; break;
	default:
		throw;
	}
#pragma endregion
}
Board Board::make_moved_board(const Move& move) const {
	Board potential_board = *this;
	potential_board.apply_move(move);
	return potential_board;
}


vector<Move> Board::movable_cases() const {
	vector<Move> result;

	for (Rank rank = RANK_1; rank <= RANK_8; ++rank) {
		for (File file = FILE_A; file <= FILE_H; ++file) {
			Coord src_crd = Coord(rank, file);

			if (get_side(src_crd) == turn);
			else
				continue;

			vector<Move> add;
			//���� ���� ������ ���� ������ ����� ���� ����ϴ� �Լ���
			//	�޸� �����Ͽ� ���� �� ���� add�� �����Ѵ�.
			if (get_on_piece(WK, src_crd) || get_on_piece(BK, src_crd))			add = movable_k(src_crd, turn);
			else if (get_on_piece(WQ, src_crd) || get_on_piece(BQ, src_crd))	add = movable_q(src_crd, turn);
			else if (get_on_piece(WR, src_crd) || get_on_piece(BR, src_crd))	add = movable_r(src_crd, turn);
			else if (get_on_piece(WB, src_crd) || get_on_piece(BB, src_crd))	add = movable_b(src_crd, turn);
			else if (get_on_piece(WN, src_crd) || get_on_piece(BN, src_crd))	add = movable_n(src_crd, turn);
			else if (get_on_piece(WP, src_crd) || get_on_piece(BP, src_crd))	add = movable_p(src_crd, turn);
			//add�� ��ҵ��� result�� �߰��ȴ�.
			result.insert(result.end(),
				add.begin(), add.end());
		}
	}
	return result;
}
inline vector<Move> Board::__movable_k(const Coord& src_crd, Side side) const {
	vector<Move> result;
	constexpr int move_r[8] = { +1,+1,+1,+0,+0,-1,-1,-1 };
	constexpr int move_f[8] = { -1,+0,+1,-1,+1,-1,+0,+1 };
	for (int i = 0; i < 8; ++i) {
		Coord dst_crd = Coord(
			src_crd.rank + move_r[i],
			src_crd.file + move_f[i]);
		if (dst_crd.on_board() && movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
		}
	}
	return result;
}
inline vector<Move> Board::movable_k(const Coord& src_crd, Side side) const {
	vector<Move> result;

	vector<Move> __result = __movable_k(src_crd, side);
	result.insert(result.begin(),
		__result.begin(), __result.end());

	//ĳ������ ������ ���, ĳ������ ������ ��츦 �߰��Ѵ�.
	//������ ����, ĳ������ ŷ �Ӹ��� �ƴ϶� ����� �����̴� Ư���� ��Ģ�̶�� ���̴�.
	//���� �̶� ĳ������ �Ϲ������� 1ĭ���� �����̴� ŷ�� 2ĭ�� �����δ�.
	//	����, ĳ������ ŷ�� 2ĭ �����̴� ���� �״�� �־� ������ �ȴ�!
	if (side == White) {
		if (castling_K &&
			get_side(Coord(RANK_1, FILE_F)) == Empty &&
			get_side(Coord(RANK_1, FILE_G)) == Empty &&
			!check(Coord(RANK_1, FILE_E), side) &&
			!check(Coord(RANK_1, FILE_F), side) &&
			!check(Coord(RANK_1, FILE_G), side)) {
			result.push_back(Move(
				Coord(RANK_1, FILE_E),
				Coord(RANK_1, FILE_G)));
		}
		if (castling_Q &&
			get_side(Coord(RANK_1, FILE_C)) == Empty &&
			get_side(Coord(RANK_1, FILE_D)) == Empty &&
			!check(Coord(RANK_1, FILE_C), side) &&
			!check(Coord(RANK_1, FILE_D), side) &&
			!check(Coord(RANK_1, FILE_E), side)) {
			result.push_back(Move(
				Coord(RANK_1, FILE_E),
				Coord(RANK_1, FILE_C)));
		}
	}
	else {
		if (castling_k &&
			get_side(Coord(RANK_8, FILE_F)) == Empty &&
			get_side(Coord(RANK_8, FILE_G)) == Empty &&
			!check(Coord(RANK_8, FILE_E), side) &&
			!check(Coord(RANK_8, FILE_F), side) &&
			!check(Coord(RANK_8, FILE_G), side)) {
			result.push_back(Move(
				Coord(RANK_8, FILE_E),
				Coord(RANK_8, FILE_G)));
		}
		if (castling_q &&
			get_side(Coord(RANK_8, FILE_C)) == Empty &&
			get_side(Coord(RANK_8, FILE_D)) == Empty &&
			!check(Coord(RANK_8, FILE_C), side) &&
			!check(Coord(RANK_8, FILE_D), side) &&
			!check(Coord(RANK_8, FILE_E), side)) {
			result.push_back(Move(
				Coord(RANK_8, FILE_E),
				Coord(RANK_8, FILE_C)));
		}
	}

	return result;
}
inline vector<Move> Board::movable_q(const Coord& src_crd, Side side) const {
	vector<Move> result;
	vector<Move> b_movable = movable_b(src_crd, side);
	vector<Move> r_movable = movable_r(src_crd, side);
	result.insert(result.end(), b_movable.begin(), b_movable.end());
	result.insert(result.end(), r_movable.begin(), r_movable.end());
	return result;
}
inline vector<Move> Board::movable_r(const Coord& src_crd, Side side) const {
	vector<Move> result;
	for (Rank rank = src_crd.rank + 1; rank <= RANK_8; ++rank) {
		Coord dst_crd = Coord(rank, src_crd.file);
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != Empty)
				break;
		}
		else
			break;
	}
	for (File file = src_crd.file + 1; file <= FILE_H; ++file) {
		Coord dst_crd = Coord(src_crd.rank, file);
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != Empty)
				break;
		}
		else
			break;
	}
	for (Rank rank = src_crd.rank - 1; rank >= RANK_1; --rank) {
		Coord dst_crd = Coord(rank, src_crd.file);
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != Empty)
				break;
		}
		else
			break;
	}
	for (File file = src_crd.file - 1; file >= FILE_A; --file) {
		Coord dst_crd = Coord(src_crd.rank, file);
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != Empty)
				break;
		}
		else
			break;
	}
	return result;
}
inline vector<Move> Board::movable_b(const Coord& src_crd, Side side) const {
	vector<Move> result;
	for (Coord dst_crd{ src_crd.rank + 1, src_crd.file - 1 }; dst_crd.on_board(); ++dst_crd.rank, --dst_crd.file) {
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != Empty)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank + 1, src_crd.file + 1 }; dst_crd.on_board(); ++dst_crd.rank, ++dst_crd.file) {
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != Empty)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank - 1, src_crd.file - 1 }; dst_crd.on_board(); --dst_crd.rank, --dst_crd.file) {
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != Empty)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank - 1, src_crd.file + 1 }; dst_crd.on_board(); --dst_crd.rank, ++dst_crd.file) {
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != Empty)
				break;
		}
		else
			break;
	}
	return result;
}
inline vector<Move> Board::movable_n(const Coord& src_crd, Side side) const {
	vector<Move> result;
	constexpr int move_r[8] = { +2,+2,+1,-1,-2,-2,-1,+1, };
	constexpr int move_f[8] = { -1,+1,+2,+2,+1,-1,-2,-2, };
	for (int i = 0; i < 8; ++i) {
		Coord dst_crd = Coord(
			src_crd.rank + move_r[i],
			src_crd.file + move_f[i]);
		if (dst_crd.on_board() && movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
		}
	}
	return result;
}
inline vector<Move> Board::movable_p(const Coord& src_crd, Side side) const {
	vector<Move> result;
	if (side == White) {
		//�� ĭ ����
		Coord dst_crd = Coord(src_crd.rank + 1, src_crd.file);
		if (dst_crd.on_board() && get_side(dst_crd) == Empty) {
			result.push_back(Move(src_crd, dst_crd));

			//�� ĭ ���� ������ ��� (�� ĭ ������ ���¿���) �� ĭ �߰� ����
			if (src_crd.rank == RANK_2) {
				++dst_crd.rank;
				if (get_side(dst_crd) == Empty)
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
		//�� ĭ ����
		Coord dst_crd = Coord(src_crd.rank - 1, src_crd.file);
		if (dst_crd.on_board() && get_side(dst_crd) == Empty) {
			result.push_back(Move(src_crd, dst_crd));

			//�� ĭ ���� ������ ��� (�� ĭ ������ ���¿���) �� ĭ �߰� ����
			if (src_crd.rank == RANK_7) {
				--dst_crd.rank;
				if (get_side(dst_crd) == Empty)
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


void Board::print(const Move& move) const {
	using std::cout;
	const std::function<void(int)> set_color = [](int color) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		};

	system("cls");
	cout << "     A   B   C   D   E   F   G   H	 \n";
	cout << "   +---+---+---+---+---+---+---+---+   \n";
	for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
		cout << ' ' << char('1' + rank) << ' ';
		for (File file = FILE_A; file <= FILE_H; ++file) {
			cout << "|";
			Coord coord = Coord(rank, file);
			if (false);
			else if (get_side(Coord(rank, file)) == White) set_color(0xF0);
			else if (get_side(Coord(rank, file)) == Black) set_color(0x80);
			if (turn == White) {
				if (move.src == coord) set_color(0x30);
				if (move.dst == coord) set_color(0x10);
			}
			if (turn == Black) {
				if (move.src == coord) set_color(0x50);
				if (move.dst == coord) set_color(0xC0);
			}
			cout << " " << get_piece(Coord(rank, file)) << " ";
			set_color(0x07);
		}
		cout << "| " << char('1' + rank) << " \n";
		cout << "   +---+---+---+---+---+---+---+---+   \n";
	}
	cout << "     A   B   C   D   E   F   G   H	 \n";
	cout << "\n";

	cout << "fen = \"" << to_fen() << "\"\n";
	cout << "turn\t\t" << (turn == White ? "WHITE" : "BLACK") << '\n';
	cout << "castling KQkq\t"
		<< (castling_K ? '1' : '-') << ' ' << (castling_Q ? '1' : '-') << ' '
		<< (castling_k ? '1' : '-') << ' ' << (castling_q ? '1' : '-') << ' ' << '\n';
	cout << "en passant\t";
	if (en_passant == Coord()) cout << '-';
	else {
		cout << char('a' + en_passant.file);
		cout << char('1' + en_passant.rank);
	}
	cout << '\n';
	cout << "half move\t" << half_move << '\n';
	cout << "full move\t" << full_move << '\n';
}
void Board::print_movable_cases(Side side) const {
	vector<Move> moves = movable_cases();
	for (Move move : moves) {
		print(move);

		while (true) {
			char order = _getch();
			if (order == ' ')
				break;
		}
	}
}


//static Move get_player_input() {
//	constexpr int arrow = -32;
//	constexpr int enter = 13;
//	constexpr int space = ' ';
//	constexpr int esc = 27;
//
//	Coord src, dst;
//
//	char order = '\0';
//	while (true) {
//#pragma region input process
//		order = _getch();
//		if ('A' <= order && order <= 'Z')
//			order += 'a' - 'A';
//		switch (order) {
//		case arrow: {
//			order = _getch();
//			switch (order) {
//			case 72: order = 'w';
//			case 75: order = 'a';
//			case 80: order = 's';
//			case 77: order = 'd';
//			}
//			break;
//		}
//		case space: {
//			order = enter;
//			break;
//		}
//		}
//#pragma endregion
//	}
//}
void Board::GAME(const string& fen) {
	using std::cout;
	Board board = Board(fen);
	char order;

	//������� �Է��� �޴� �Լ�.
	//ȭ��ǥ �Է��� wasd�� ��ȯ���ִ� ������ �����̴�.
	std::function<char()> get_order = []() {
		char order = _getch();
		if (order == -32) {
			order = _getch();
			switch (order) {
			case 72: return 'w';
			case 75: return 'a';
			case 80: return 's';
			case 77: return 'd';
			}
		}
		if (order == 13)
			return ' ';
		return order;
		};
	//�Է¹��� ��ǥ�� �� ĭ �̵���Ű�� �Լ�.
	//order�� ����� ���� �������� �̵��Ѵ�.
	//ü������ ����� �̵��� �� ���� �����ߴ�.
	std::function<void(Coord&)> move = [&order](Coord& crd) {
		switch (order) {
		case 'W': case 'w': if (crd.rank < RANK_8) ++crd.rank; break;
		case 'A': case 'a': if (crd.file > FILE_A) --crd.file; break;
		case 'S': case 's': if (crd.rank > RANK_1) --crd.rank; break;
		case 'D': case 'd': if (crd.file < FILE_H) ++crd.file; break;
		}
		};

	while (true) {
		//src, dst�� ���� �������� ����������,
		//�ٽ� ���� �⹰, �׸��� �⹰�� ������ ��ġ�̴�.
		Coord src = Coord(), dst = Coord();
		board.print();

		//���ʰ� ���̸� ���� �Ʒ�����, ���̸� ���� ������ �����ϵ��� �ߴ�.
		src = (board.turn == White) ? Coord(RANK_1, FILE_A) : Coord(RANK_8, FILE_A);
		//dst�� ���� �������� ��Ҹ� ������ ��, ���ƿ��� ���� goto ���̴�.
		game_src_cancel:
		order = '\0';
		//order�� space �̰ų� enter�� ��� ������ �����ϵ��� �ߴ�
		while (true) {
			//src�� dst�� �ݿ��� �� ��Ȳ�� ȭ�鿡 ����Ѵ�.
			board.print(Move(src, dst));
#pragma region src input
			order = get_order();
			move(src);
			if (order == 'Q' || order == 'q');
			//order�� esc�� ��� GAME() �Լ��� �����Ѵ�.
			else if (order == 27)	return;
#pragma endregion
#pragma region src check legal
			//ü�ʿ� �´� �⹰�� ��ġ�� �������� �ʾ��� ���, �ٽ� �����ϵ��� �Ѵ�.
			else if (order == ' ') {
				if (board.get_side(src) != board.turn) {
					std::cout << '\n';
					std::cout << "�߸��� ������ �����߽��ϴ�." << '\n';
					std::cout << "�ùٸ� ������ �⹰�� ������ �ּ���." << '\n';
					std::cout << "(Press any button to continue.)" << '\n';
					_getch();
				}
				else
					break;
			}
#pragma endregion
		}

		//dst�� �ʱ� ��ġ�� src�� �����Ѵ�.
		dst = src;
		order = '\0';
		while (order != ' '&& order != 13) {
			//src�� dst�� �ݿ��� �� ��Ȳ�� ȭ�鿡 ����Ѵ�.
			board.print(Move(src, dst));
#pragma region dst input
			order = get_order();
			move(dst);
			if (order == 'Q' || order == 'q') {
				dst = Coord();
				//src�� �����ϴ� ��ġ�� �ǵ��ư���.
				goto game_src_cancel;
			}
			else if (order == 27)	return;
#pragma endregion
#pragma region dst check legal
			else if (order == ' ') {
				//������ �� �ִ� �����ӵ� ���
				vector<Move> moves;
				switch (board.get_piece(src)) {
				case 'K': case 'k': moves = board.movable_k(src, board.turn); break;
				case 'Q': case 'q': moves = board.movable_q(src, board.turn); break;
				case 'R': case 'r': moves = board.movable_r(src, board.turn); break;
				case 'B': case 'b': moves = board.movable_b(src, board.turn); break;
				case 'N': case 'n': moves = board.movable_n(src, board.turn); break;
				case 'P': case 'p': moves = board.movable_p(src, board.turn); break;
				}
				//��ǥ�ϴ� �������� ���� �����ӿ� ���ԵǴ°� Ȯ��
				bool is_coord_legal = true;
				Move move = Move(src, dst);
				if (find(moves.begin(), moves.end(), move) == moves.end())
					is_coord_legal = false;
				//������ �� �Ǵ��� ���� ������
				bool is_pin_legal = true;
				Board new_board = board.make_moved_board(move);
				Side check_side = new_board.check();
				if (check_side == board.turn ||
					check_side == Grey)
					is_pin_legal = false;
				//�̵��� �������� ���� Ȯ��
				if (is_coord_legal && is_pin_legal)
					break;
				else {
					cout << '\n';
					cout << "�߸��� ��ġ�� �����߽��ϴ�." << '\n';
					if (!is_coord_legal)
						cout << "���� : �� �⹰�� ������ ��ġ�� �̵��� �� ����\n";
					if (!is_pin_legal)
						cout << "���� : ������ �� ���¿� ����\n";
					cout << "(Press any button to continue.)" << '\n';
					_getch();
				}
			}
#pragma endregion
		}

		//�÷��̰Ű� ������ �̵��� �����Ѵ�.
		board.apply_move(Move(src, dst));

		//���θ�� ������ ���, ���ϴ� ���� �����ϵ��� �Ѵ�.
		char piece = board.get_piece(dst);
		if (piece == 'P' && dst.rank == RANK_8 ||
			piece == 'p' && dst.rank == RANK_1) {
			cout << "���θ�� �����մϴ�.\n";
			cout << "��ü�� ���ϴ� �⹰�� ������ �ּ���.\n";
			cout << "\t(Q : ��, R : ��, B : ���, N : ����Ʈ, 0 : ���)\n";
			char order = '\0';
			bool input_process = true;
			while (input_process) {
				order = _getch();
				//order�� �빮���� ���, �ҹ��ڷ� ��ȯ���ش�.
				switch (order) {
				case 'Q': board.set_on_piece(WQ, dst); break;
				case 'R': board.set_on_piece(WR, dst); break;
				case 'B': board.set_on_piece(WB, dst); break;
				case 'N': board.set_on_piece(WN, dst); break;

				case 'q': board.set_on_piece(BQ, dst); break;
				case 'r': board.set_on_piece(BR, dst); break;
				case 'b': board.set_on_piece(BB, dst); break;
				case 'n': board.set_on_piece(BN, dst); break;

				case '0':
					//0�� �Է����� ���, �״�� �ݺ����� �����ϵ��� �Ѵ�.
					input_process = false;
					break;
				default:
					//������ ���� �Է��� �־����� ���, �ٽ� �����ϵ��� �Ѵ�.
					cout << "�߸��� �Է��Դϴ�.\n";
					break;
				}
			}
		}
	}
}
/*
	< Ư����Ģ ��Ȳ Ŀ��Ʈ >

���Ļ� :
	"w ww d ssa www w ds ss wwww dw "

ŷ���̵� ĳ���� :
	"dddddddw ww s s ddddddw ww ss s ddddd dw ds s dddddd dww dss s `dddd dd "
	"ddddddw ww dddddds ss ddddd dwdw ddddd dsds dddddd wwa dddddd ssa `dddd dd "

�����̵� ĳ���� :
	"d wwa s s dw ww ss s dd aw ds s ddw w dss s ddd aw dds s `dddd aa "

*/


void Board::BOT_GAME(const string& fen) {
#pragma region constant values
	constexpr int arrow = -32;
	constexpr int enter = 13;
	constexpr int space = ' ';
	constexpr int esc = 27;
#pragma endregion
	using std::cout;
	Board board = Board(fen);
	char order;
	Coord src = Coord();
	Coord dst = Coord();
	PruningTree p_tree;

	while (true) {
		src = Coord(RANK_1, FILE_A);
		dst = Coord();
		board.print();

		//�÷��̾�1 - �ΰ�
#pragma region input process 1
		bot_game_src_cancel:
		order = '\0';
		while (true) {
			board.print(Move(src, dst));
			order = _getch();
			switch (order) {
			case arrow: {
				order = _getch();
				switch (order) {
				case 72: order = 'w'; break;
				case 75: order = 'a'; break;
				case 80: order = 's'; break;
				case 77: order = 'd'; break;
				}
				break;
			}
			case space: {
				order = enter;
				break;
			}
			case esc:
				return;
			}
			switch (order) {
			case 'W': case 'w': if (src.rank < RANK_8) ++src.rank; break;
			case 'A': case 'a': if (src.file > FILE_A) --src.file; break;
			case 'S': case 's': if (src.rank > RANK_1) --src.rank; break;
			case 'D': case 'd': if (src.file < FILE_H) ++src.file; break;
			}
			if (order == enter) {
				if (board.get_side(src) != board.turn) {
					std::cout << '\n';
					std::cout << "�߸��� ������ �����߽��ϴ�." << '\n';
					std::cout << "�ùٸ� ������ �⹰�� ������ �ּ���." << '\n';
					std::cout << "(Press any button to continue.)" << '\n';
					_getch();
				}
				else
					break;
			}
		}
#pragma endregion
#pragma region input process 2
		dst = src;
		order = '\0';
		while (true) {
			board.print(Move(src, dst));
			order = _getch();
			switch (order) {
			case arrow: {
				order = _getch();
				switch (order) {
				case 72: order = 'w'; break;
				case 75: order = 'a'; break;
				case 80: order = 's'; break;
				case 77: order = 'd'; break;
				}
				break;
			}
			case space: {
				order = enter;
				break;
			}
			case esc:
				return;
			}
			switch (order) {
			case 'W': case 'w': if (dst.rank < RANK_8) ++dst.rank; break;
			case 'A': case 'a': if (dst.file > FILE_A) --dst.file; break;
			case 'S': case 's': if (dst.rank > RANK_1) --dst.rank; break;
			case 'D': case 'd': if (dst.file < FILE_H) ++dst.file; break;
			case 'Q': case 'q':
				dst = Coord();
				goto bot_game_src_cancel;
				break;
			}
			if (order == enter) {
				//������ �� �ִ� �����ӵ� ���
				vector<Move> moves;
				if (board.get_on_piece(WK, src) || board.get_on_piece(BK, src))			moves = board.movable_k(src, board.turn);
				else if (board.get_on_piece(WQ, src) || board.get_on_piece(BQ, src))	moves = board.movable_q(src, board.turn);
				else if (board.get_on_piece(WR, src) || board.get_on_piece(BR, src))	moves = board.movable_r(src, board.turn);
				else if (board.get_on_piece(WB, src) || board.get_on_piece(BB, src))	moves = board.movable_b(src, board.turn);
				else if (board.get_on_piece(WN, src) || board.get_on_piece(BN, src))	moves = board.movable_n(src, board.turn);
				else if (board.get_on_piece(WP, src) || board.get_on_piece(BP, src))	moves = board.movable_p(src, board.turn);
				//��ǥ�ϴ� �������� ���� �����ӿ� ���ԵǴ°� Ȯ��
				bool is_coord_legal = true;
				Move move = Move(src, dst);
				if (find(moves.begin(), moves.end(), move) == moves.end())
					is_coord_legal = false;
				//������ �� �Ǵ��� ���� ������
				bool is_pin_legal = true;
				Board new_board = board.make_moved_board(move);
				Side check_side = new_board.check();
				if (check_side == board.turn ||
					check_side == Grey)
					is_pin_legal = false;
				//�̵��� �������� ���� Ȯ��
				if (is_coord_legal && is_pin_legal)
					break;
				else {
					cout << '\n';
					cout << "�߸��� ��ġ�� �����߽��ϴ�." << '\n';
					if (!is_coord_legal)
						cout << "���� : �� �⹰�� ������ ��ġ�� �̵��� �� ����\n";
					if (!is_pin_legal)
						cout << "���� : ������ �� ���¿� ����\n";
					cout << "(Press any button to continue.)" << '\n';
					_getch();
				}
			}
		}
#pragma endregion
		board.apply_move(Move(src, dst));

		//�÷��̾�2 - ���
#pragma region input process
		board.print();
		Move p_move = p_tree.get_nxt_move(board);
		board.print(p_move);
#pragma endregion
		board.apply_move(p_move);
	}
}
