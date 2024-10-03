#include <functional>
#include "Board.h"

//! 디버그용 - print(), print_movable_cases() 함수에만 사용
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

	//기물의 위치
	Rank rank = RANK_8;
	File file = FILE_A;
	for (char c : position_str) {
		if (c == ' ') {
			break;
		}
		else if (c == '/'); //c가 '/'인 경우는 무시한다.
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

	//차례
	switch (turn_str[0]) {
	case 'W': case 'w': turn = WHITE; break;
	case 'B': case 'b': turn = BLACK; break;
	default:
		break;
	}

	//캐슬링 가능 여부
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

	//앙파상 가능한 칸
	if (en_passant_str != "-") {
		if (en_passant_str[0] >= 'A' &&
			en_passant_str[0] <= 'Z')
			en_passant_str[0] += 'a' - 'A';
		en_passant = Coord(
			RANK_1 + (en_passant_str[1] - '1'),
			FILE_A + (en_passant_str[0] - 'a')
		);
	}

	//하프무브
	if (half_move_str != "-")
		half_move = std::stoi(half_move_str);

	//풀무브
	if (full_move_str != "-")
		full_move = std::stoi(full_move_str);
}


string Board::to_fen() const {
	string fen;

	//기물의 위치
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

	//차례
	switch (turn) {
	case WHITE: fen += 'w'; break;
	case BLACK: fen += 'b'; break;
	default:	fen += '-'; break;
	}
	fen += ' ';

	//캐슬링 가능 여부
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

	//앙파상 가능한 칸
	if (en_passant == Coord()) {
		fen += '-';
	}
	else {
		fen += 'a' + en_passant.file;
		fen += '1' + en_passant.rank;
	}
	fen += ' ';

	//하프무브
	fen += std::to_string(half_move);
	fen += ' ';

	//풀무브
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
	//하프무브의 갱신
	if (get_side(move.dst) == EMPTY)
		half_move++;
	else
		half_move = 0;
	//풀무부의 갱신
	full_move += (turn == BLACK) ? 1 : 0;
#pragma endregion
#pragma region move
	//통상적인 움직임
	set_piece(move.dst, move_piece);
	set_piece(move.src, ' ');
	//캐슬링
	//	이미 movable_k()에서 캐슬링 가능 여부를 판단했으니, 따로 확인하지 않는다
	//	킹은 이미 움직였기 때문에 룩만 추가적으로 움직인다.
	if (move_piece == 'K') {
		//백 킹사이드
		if (castling_K && move.dst == Coord(RANK_1, FILE_G)) {
			move_piece_to(
				Coord(RANK_1, FILE_H),
				Coord(RANK_1, FILE_F));
			castling_K = false;
			castling_Q = false;
		}
		//백 퀸사이드
		if (castling_Q && move.dst == Coord(RANK_1, FILE_C)) {
			move_piece_to(
				Coord(RANK_1, FILE_A),
				Coord(RANK_1, FILE_D));
			castling_K = false;
			castling_Q = false;
		}
	}
	if (move_piece == 'k') {
		//흑 킹사이드
		if (castling_k && move.dst == Coord(RANK_8, FILE_G)) {
			move_piece_to(
				Coord(RANK_8, FILE_H),
				Coord(RANK_8, FILE_F));
			castling_k = false;
			castling_q = false;
		}
		//흑 퀸사이드
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

	//캐슬링 정보의 갱신
	switch (move_piece) {
	case 'K':
		castling_K = false;
		castling_Q = false;
		break;
	case 'R':
		if (move.src == Coord(RANK_1, FILE_H)) castling_K = false; //킹사이드
		if (move.src == Coord(RANK_1, FILE_A)) castling_Q = false; //퀀사이드
		break;

	case 'k':
		castling_k = false;
		castling_q = false;
		break;
	case 'r':
		if (move.src == Coord(RANK_8, FILE_H)) castling_k = false; //킹사이드
		if (move.src == Coord(RANK_8, FILE_A)) castling_q = false; //퀀사이드
		break;

	default:
		break;
	}

	//앙파상 위치의 갱신
	en_passant = Coord();
	if (move_piece == 'P' &&
		move.src.rank == RANK_2 &&
		move.dst.rank == RANK_4)
		en_passant = Coord(RANK_3, move.src.file);
	if (move_piece == 'p' &&
		move.src.rank == RANK_7 &&
		move.dst.rank == RANK_5)
		en_passant = Coord(RANK_3, move.src.file);

	//차례의 전환
	switch (turn) {
	case WHITE: turn = BLACK; break;
	case BLACK: turn = WHITE; break;
	default:
		throw;
	}
#pragma endregion
}


//나중에 movable() 참조 없이 돌아가도록 다시 짜 보자.
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


//캐슬링 적용
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

	//캐슬링이 가능할 경우, 캐슬링을 실행한 경우를 추가한다.
	//주의할 것은, 캐슬링은 킹 뿐만이 아니라 룩까지 움직이는 특수한 규칙이라는 것이다.
	//또한 이때 캐슬링은 일반적으로 1칸만을 움직이던 킹이 2칸을 움직인다.
	//	따라서, 캐슬링은 킹이 2칸 움직이는 것을 그대로 넣어 버리면 된다!
	if (side == WHITE) {
		if (castling_K) {
			//조건 달성 확인 - 3번 조건은 체크 함수를 통해 구하면 될 것 같다.(그렇게 하려면 당연히 체크 함수를 다시 만들어야 하겠지만)
			result.push_back(Move(
				Coord(RANK_1, FILE_E),
				Coord(RANK_1, FILE_G)));
		}
		if (castling_Q) {
			//조건 달성 확인
			result.push_back(Move(
				Coord(RANK_1, FILE_E),
				Coord(RANK_1, FILE_C)));
		}
	}
	else {
		if (castling_k) {
			//조건 달성 확인
			result.push_back(Move(
				Coord(RANK_8, FILE_E),
				Coord(RANK_8, FILE_G)));
		}
		if (castling_q) {
			//조건 달성 확인
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
