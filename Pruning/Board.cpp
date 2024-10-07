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
vector<Move> Board::movable_cases() const {
	vector<Move> result;

	for (Rank rank = RANK_1; rank <= RANK_8; ++rank) {
		for (File file = FILE_A; file <= FILE_H; ++file) {
			Coord src_crd = Coord(rank, file);

			if (get_side(src_crd) == turn);
			else
				continue;

			vector<Move> add;
			//놓인 말의 종류에 따라 움질임 경우의 수를 계산하는 함수를
			//	달리 구현하여 실행 및 변수 add에 대입한다.
			switch (get_piece(src_crd)) {
			case 'K': case 'k': { add = movable_k(src_crd, turn); break; }
			case 'Q': case 'q': { add = movable_q(src_crd, turn); break; }
			case 'R': case 'r': { add = movable_r(src_crd, turn); break; }
			case 'B': case 'b': { add = movable_b(src_crd, turn); break; }
			case 'N': case 'n': { add = movable_n(src_crd, turn); break; }
			case 'P': case 'p': { add = movable_p(src_crd, turn); break; }
			}
			//add의 요소들은 result에 추가된다.
			result.insert(result.end(),
				add.begin(), add.end());
		}
	}
	return result;
}


void Board::print(Move move) const {
	using std::cout;
	const std::function<void(int)> set_color = [](int color) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		};

	system("cls");
	cout << "	 A   B   C   D   E   F   G   H	 \n";
	cout << "   +---+---+---+---+---+---+---+---+   \n";
	for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
		cout << ' ' << char('1' + rank) << ' ';
		for (File file = FILE_A; file <= FILE_H; ++file) {
			cout << "|";
			Coord coord = Coord(rank, file);
			if (false);
			else if (get_side(Coord(rank, file)) == WHITE) set_color(0xF0);
			else if (get_side(Coord(rank, file)) == BLACK) set_color(0x80);
			if (turn == WHITE) {
				if (move.src == coord) set_color(0x30);
				if (move.dst == coord) set_color(0x10);
			}
			if (turn == BLACK) {
				if (move.src == coord) set_color(0x50);
				if (move.dst == coord) set_color(0xC0);
			}
			cout << " " << get_piece(Coord(rank, file)) << " ";
			set_color(0x07);
		}
		cout << "| " << char('1' + rank) << " \n";
		cout << "   +---+---+---+---+---+---+---+---+   \n";
	}
	cout << "	 A   B   C   D   E   F   G   H	 \n";
	cout << "\n";

	cout << "fen = \"" << to_fen() << "\"\n";
	cout << "turn\t\t" << (turn == WHITE ? "WHITE" : "BLACK") << '\n';
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
void Board::GAME(string fen) {
	using std::cout;
	Board board = Board(fen);
	char order;

	//사용자의 입력을 받는 함수.
	//화살표 입력을 wasd로 전환해주는 역할이 메인이다.
	std::function<char()> get_order = []() {
		char order = _getch();
		if (order == -32) {
			order = _getch();
			switch (order) {
			case 72: return 'w';
			case 75: return 'a';
			case 80: return 's';
			case 77: return 'd';
			case 13: return ' ';
			}
		}
		return order;
		};
	//입력받은 좌표를 한 칸 이동시키는 함수.
	//order의 방향과 같은 방향으로 이동한다.
	//체스판을 벗어나서 이동할 수 없게 설정했다.
	std::function<void(Coord&)> move = [&order](Coord& crd) {
		switch (order) {
		case 'W': case 'w': if (crd.rank < RANK_8) ++crd.rank; break;
		case 'A': case 'a': if (crd.file > FILE_A) --crd.file; break;
		case 'S': case 's': if (crd.rank > RANK_1) --crd.rank; break;
		case 'D': case 'd': if (crd.file < FILE_H) ++crd.file; break;
		}
		};

	while (true) {
		//src, dst는 각각 시작접과 도착점으로,
		//다시 말해 기물, 그리고 기물이 도달할 위치이다.
		Coord src = Coord(), dst = Coord();
		board.print();

		//차례가 백이면 왼쪽 아래에서, 흑이면 왼쪽 위에서 시작하도록 했다.
		src = (board.turn == WHITE) ? Coord(RANK_1, FILE_A) : Coord(RANK_8, FILE_A);
		//dst의 선택 과정에서 취소를 택했을 때, 돌아오기 위한 goto 라벨이다.
		game_src_cancel:
		order = '\0';
		//order가 space 이거나 enter인 경우 선택을 종료하도록 했다
		while (true) {
			//src와 dst를 반영해 현 상황을 화면에 출력한다.
			board.print(Move(src, dst));
#pragma region src input
			order = get_order();
			move(src);
			if (order == 'Q' || order == 'q');
			//order가 esc인 경우 GAME() 함수를 종료한다.
			else if (order == 27)	return;
#pragma endregion
#pragma region src check legal
			//체례에 맞는 기물의 위치를 선택하지 않았을 경우, 다시 선택하도록 한다.
			else if (order == ' ') {
				if (board.get_side(src) != board.turn) {
					std::cout << '\n';
					std::cout << "잘못된 진영을 선택했습니다." << '\n';
					std::cout << "올바른 진영의 기물을 선택해 주세요." << '\n';
					std::cout << "(Press any button to continue.)" << '\n';
					_getch();
				}
				else
					break;
			}
#pragma endregion
		}

		//dst의 초기 위치를 src로 지정한다.
		dst = src;
		order = '\0';
		while (order != ' '&& order != 13) {
			//src와 dst를 반영해 현 상황을 화면에 출력한다.
			board.print(Move(src, dst));
#pragma region dst input
			order = get_order();
			move(dst);
			if (order == 'Q' || order == 'q') {
				dst = Coord();
				//src가 시작하는 위치로 되돌아간다.
				goto game_src_cancel;
			}
			else if (order == 27)	return;
#pragma endregion
#pragma region dst check legal
			else if (order == ' ') {
				//움직일 수 있는 움직임들 계산
				vector<Move> moves;
				switch (board.get_piece(src)) {
				case 'K': case 'k': moves = board.movable_k(src, board.turn); break;
				case 'Q': case 'q': moves = board.movable_q(src, board.turn); break;
				case 'R': case 'r': moves = board.movable_r(src, board.turn); break;
				case 'B': case 'b': moves = board.movable_b(src, board.turn); break;
				case 'N': case 'n': moves = board.movable_n(src, board.turn); break;
				case 'P': case 'p': moves = board.movable_p(src, board.turn); break;
				}
				//목표하는 움직임이 위의 움직임에 포함되는가 확인
				bool is_coord_legal = true;
				Move move = Move(src, dst);
				if (find(moves.begin(), moves.end(), move) == moves.end())
					is_coord_legal = false;
				//절대적 핀 판단을 위한 변수들
				bool is_pin_legal = true;
				Board new_board = board.make_moved_board(move);
				Side check_side = new_board.check();
				if (check_side == board.turn ||
					check_side == GREY)
					is_pin_legal = false;
				//이동이 적법한지 최종 확인
				if (is_coord_legal && is_pin_legal)
					break;
				else {
					cout << '\n';
					cout << "잘못된 위치를 선택했습니다." << '\n';
					if (!is_coord_legal)
						cout << "사유 : 이 기물은 선택한 위치로 이동할 수 없음\n";
					if (!is_pin_legal)
						cout << "사유 : 절대적 핀 상태에 놓임\n";
					cout << "(Press any button to continue.)" << '\n';
					_getch();
				}
			}
#pragma endregion
		}

		//플레이거가 선택한 이동을 적용한다.
		board.apply_move(Move(src, dst));

		//프로모션 가능한 경우, 원하는 말을 선택하도록 한다.
		char piece = board.get_piece(dst);
		if (piece == 'P' && dst.rank == RANK_8 ||
			piece == 'p' && dst.rank == RANK_1) {
			cout << "프로모션 가능합니다.\n";
			cout << "교체를 원하는 기물을 선택해 주세요.\n";
			cout << "\t(Q : 퀸, R : 룩, B : 비숍, N : 나이트, 0 : 취소)\n";
			char order = '\0';
			bool input_process = true;
			while (input_process) {
				order = _getch();
				//order가 대문자인 경우, 소문자로 변환해준다.
				if ('A' <= order && order <= 'Z')
					order += 'a' - 'A';
				switch (order) {
				case 'q':
				case 'r':
				case 'b':
				case 'n':
					//프로모션으로서 바꿀 기물을 선택했을 경우, 선택한 대로 기물을 바꿔준다.
					board.set_piece(dst, piece + order - 'p');
				case '0':
					//0을 입력했을 경우, 그대로 반복문을 적용하도록 한다.
					input_process = false;
					break;
				default:
					//선택지 밖의 입력이 주어졌을 경우, 다시 선택하도록 한다.
					cout << "잘못된 입력입니다.\n";
					break;
				}
			}
		}
	}
}
/*
	< 특수규칙 상황 커멘트 >

앙파상 :
	"w ww d ssa www w ds ss wwww dw "

킹사이드 캐슬링 :
	"dddddddw ww s s ddddddw ww ss s ddddd dw ds s dddddd dww dss s `dddd dd "
	"ddddddw ww dddddds ss ddddd dwdw ddddd dsds dddddd wwa dddddd ssa `dddd dd "

퀸사이드 캐슬링 :
	"d wwa s s dw ww ss s dd aw ds s ddw w dss s ddd aw dds s `dddd aa "

*/


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


inline bool Board::movable_piece(const Coord& dst_crd) const {
	if (get_side(dst_crd) != turn)
		return true;
	else
		return false;
}
inline void Board::move_piece_to(Coord src_crd, Coord dst_crd) {
	const char move_piece = get_piece(src_crd);
	set_piece(dst_crd, move_piece);
	set_piece(src_crd, ' ');
}
inline void Board::apply_move(Move move) {
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
	//앙파상 적용
	if (en_passant == move.dst && (
		move_piece == 'P' ||
		move_piece == 'p')) {
		if (en_passant.rank == RANK_3)
			set_piece(Coord(RANK_4, en_passant.file), ' ');
		if (en_passant.rank == RANK_6)
			set_piece(Coord(RANK_5, en_passant.file), ' ');
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
		en_passant = Coord(RANK_6, move.src.file);

	//차례의 전환
	switch (turn) {
	case WHITE: turn = BLACK; break;
	case BLACK: turn = WHITE; break;
	default:
		throw;
	}
#pragma endregion
}


inline bool Board::check(Coord k_crd, Side side) const {
	for (Move move : __movable_k(k_crd, side))
		if (side == WHITE && get_piece(move.dst) == 'k' ||
			side == BLACK && get_piece(move.dst) == 'K')
			return true;

	for (Move move : movable_r(k_crd, side))
		if (side == WHITE && (get_piece(move.dst) == 'r' || get_piece(move.dst) == 'q') ||
			side == BLACK && (get_piece(move.dst) == 'R' || get_piece(move.dst) == 'Q'))
			return true;

	for (Move move : movable_b(k_crd, side))
		if (side == WHITE && (get_piece(move.dst) == 'b' || get_piece(move.dst) == 'q') ||
			side == BLACK && (get_piece(move.dst) == 'B' || get_piece(move.dst) == 'Q'))
			return true;

	for (Move move : movable_n(k_crd, side))
		if (side == WHITE && get_piece(move.dst) == 'n' ||
			side == BLACK && get_piece(move.dst) == 'N')
			return true;

	for (Move move : movable_p(k_crd, side))
		if (side == WHITE && get_piece(move.dst) == 'p' ||
			side == BLACK && get_piece(move.dst) == 'P')
			return true;

	return false;
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

	bool w_checked = check(w_king, WHITE);
	bool b_checked = check(b_king, BLACK);

	if (w_checked && b_checked)
		return GREY;
	else if (w_checked)	return WHITE;
	else if (b_checked)	return BLACK;
	else				return EMPTY;
}
//좀 더 최적화된 형태로 만들어야 하나...
inline bool Board::checkmate() const {
	vector<Move> moves = movable_cases();
	for (Move move : moves) {
		Board new_board = make_moved_board(move);
		Side check_side = new_board.check();
		if (check_side == turn && check_side == GREY); //더블 체크인 경우는 어떻게 되는가...
		else
			return true;
	}
	return false;
}


inline vector<Move> Board::__movable_k(Coord src_crd, Side side) const {
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
inline vector<Move> Board::movable_k(Coord src_crd, Side side) const {
	vector<Move> result;

	vector<Move> __result = __movable_k(src_crd, side);
	result.insert(result.begin(),
		__result.begin(), __result.end());

	//캐슬링이 가능할 경우, 캐슬링을 실행한 경우를 추가한다.
	//주의할 것은, 캐슬링은 킹 뿐만이 아니라 룩까지 움직이는 특수한 규칙이라는 것이다.
	//또한 이때 캐슬링은 일반적으로 1칸만을 움직이던 킹이 2칸을 움직인다.
	//	따라서, 캐슬링은 킹이 2칸 움직이는 것을 그대로 넣어 버리면 된다!
	if (side == WHITE) {
		if (castling_K &&
			get_side(Coord(RANK_1, FILE_F)) == EMPTY &&
			get_side(Coord(RANK_1, FILE_G)) == EMPTY &&
			!check(Coord(RANK_1, FILE_E), side) &&
			!check(Coord(RANK_1, FILE_F), side) &&
			!check(Coord(RANK_1, FILE_G), side)) {
			result.push_back(Move(
				Coord(RANK_1, FILE_E),
				Coord(RANK_1, FILE_G)));
		}
		if (castling_Q &&
			get_side(Coord(RANK_1, FILE_C)) == EMPTY &&
			get_side(Coord(RANK_1, FILE_D)) == EMPTY &&
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
			get_side(Coord(RANK_8, FILE_F)) == EMPTY &&
			get_side(Coord(RANK_8, FILE_G)) == EMPTY &&
			!check(Coord(RANK_8, FILE_E), side) &&
			!check(Coord(RANK_8, FILE_F), side) &&
			!check(Coord(RANK_8, FILE_G), side)) {
			result.push_back(Move(
				Coord(RANK_8, FILE_E),
				Coord(RANK_8, FILE_G)));
		}
		if (castling_q &&
			get_side(Coord(RANK_8, FILE_C)) == EMPTY &&
			get_side(Coord(RANK_8, FILE_D)) == EMPTY &&
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
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (File file = src_crd.file + 1; file <= FILE_H; ++file) {
		Coord dst_crd = Coord(src_crd.rank, file);
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Rank rank = src_crd.rank - 1; rank >= RANK_1; --rank) {
		Coord dst_crd = Coord(rank, src_crd.file);
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (File file = src_crd.file - 1; file >= FILE_A; --file) {
		Coord dst_crd = Coord(src_crd.rank, file);
		if (movable_piece(dst_crd)) {
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
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank + 1, src_crd.file + 1 }; dst_crd.on_board(); ++dst_crd.rank, ++dst_crd.file) {
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank - 1, src_crd.file - 1 }; dst_crd.on_board(); --dst_crd.rank, --dst_crd.file) {
		if (movable_piece(dst_crd)) {
			result.push_back(Move(src_crd, dst_crd));
			if (get_side(dst_crd) != EMPTY)
				break;
		}
		else
			break;
	}
	for (Coord dst_crd{ src_crd.rank - 1, src_crd.file + 1 }; dst_crd.on_board(); --dst_crd.rank, ++dst_crd.file) {
		if (movable_piece(dst_crd)) {
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
inline vector<Move> Board::movable_p(Coord src_crd, Side side) const {
	vector<Move> result;
	if (side == WHITE) {
		//한 칸 전진
		Coord dst_crd = Coord(src_crd.rank + 1, src_crd.file);
		if (dst_crd.on_board() && get_side(dst_crd) == EMPTY) {
			result.push_back(Move(src_crd, dst_crd));

			//두 칸 전진 가능할 경우 (한 칸 전진한 상태에서) 한 칸 추가 전진
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
		//한 칸 전진
		Coord dst_crd = Coord(src_crd.rank - 1, src_crd.file);
		if (dst_crd.on_board() && get_side(dst_crd) == EMPTY) {
			result.push_back(Move(src_crd, dst_crd));

			//두 칸 전진 가능할 경우 (한 칸 전진한 상태에서) 한 칸 추가 전진
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
