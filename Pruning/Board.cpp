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
			switch (get_piece(src_crd)) {
			case 'K': case 'k': { add = movable_k(src_crd, turn); break; }
			case 'Q': case 'q': { add = movable_q(src_crd, turn); break; }
			case 'R': case 'r': { add = movable_r(src_crd, turn); break; }
			case 'B': case 'b': { add = movable_b(src_crd, turn); break; }
			case 'N': case 'n': { add = movable_n(src_crd, turn); break; }
			case 'P': case 'p': { add = movable_p(src_crd, turn); break; }
			}
			//add�� ��ҵ��� result�� �߰��ȴ�.
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
			case 13: return ' ';
			}
		}
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
		src = (board.turn == WHITE) ? Coord(RANK_1, FILE_A) : Coord(RANK_8, FILE_A);
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
					check_side == GREY)
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
				if ('A' <= order && order <= 'Z')
					order += 'a' - 'A';
				switch (order) {
				case 'q':
				case 'r':
				case 'b':
				case 'n':
					//���θ�����μ� �ٲ� �⹰�� �������� ���, ������ ��� �⹰�� �ٲ��ش�.
					board.set_piece(dst, piece + order - 'p');
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
	//���Ļ� ����
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
		en_passant = Coord(RANK_6, move.src.file);

	//������ ��ȯ
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
//�� �� ����ȭ�� ���·� ������ �ϳ�...
inline bool Board::checkmate() const {
	vector<Move> moves = movable_cases();
	for (Move move : moves) {
		Board new_board = make_moved_board(move);
		Side check_side = new_board.check();
		if (check_side == turn && check_side == GREY); //���� üũ�� ���� ��� �Ǵ°�...
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

	//ĳ������ ������ ���, ĳ������ ������ ��츦 �߰��Ѵ�.
	//������ ����, ĳ������ ŷ �Ӹ��� �ƴ϶� ����� �����̴� Ư���� ��Ģ�̶�� ���̴�.
	//���� �̶� ĳ������ �Ϲ������� 1ĭ���� �����̴� ŷ�� 2ĭ�� �����δ�.
	//	����, ĳ������ ŷ�� 2ĭ �����̴� ���� �״�� �־� ������ �ȴ�!
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
		//�� ĭ ����
		Coord dst_crd = Coord(src_crd.rank + 1, src_crd.file);
		if (dst_crd.on_board() && get_side(dst_crd) == EMPTY) {
			result.push_back(Move(src_crd, dst_crd));

			//�� ĭ ���� ������ ��� (�� ĭ ������ ���¿���) �� ĭ �߰� ����
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
		//�� ĭ ����
		Coord dst_crd = Coord(src_crd.rank - 1, src_crd.file);
		if (dst_crd.on_board() && get_side(dst_crd) == EMPTY) {
			result.push_back(Move(src_crd, dst_crd));

			//�� ĭ ���� ������ ��� (�� ĭ ������ ���¿���) �� ĭ �߰� ����
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
