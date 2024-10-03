#pragma once
#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <vector>
#include <string>
#include "Coord.h"
#include "Move.h"
using std::string;
using std::vector;

const string default_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";

enum Side : char { WHITE, BLACK, EMPTY, GREY/*check 함수에서만 사용한다.*/ };

/// <summary>
/// 체스 보드를 8*8의 형태로 저장하는 구조체.
/// FEN 형식과 일대일로 대응할 수 있다.
/// <para>
///		용량이 크기 때문에 사용을 최소화해야 한다.
/// </para>
/// </summary>
class Board {
public:
	Board();
	Board(string FEN);


	/// <summary>
	/// Board의 위치 정보만을 FEN 형식으로 나타내 반환하는 함수이다.
	/// </summary>
	/// <returns> FEN 형식의 기물 위치정보 </returns>
	string to_fen() const;
	/// <summary>
	///	move라는 수를 둔 결과의 Board를 반환하는 함수이다.
	/// <para>
	///		*this를 복제해 apply_move()를 적용시키는 방식이다.
	/// </para>
	/// </summary>
	/// <param name="move"> 기물의 움직임 </param>
	/// <returns> 움직임을 적용한 결과 </returns>
	Board make_moved_board(Move move) const;
	/// <summary>
	/// 현재 Board의 상태에서 진영에 따라 움직일 수 있는 모든 경우를 반환하는 함수이다.
	/// </summary>
	/// <param name="is_white_side"></param>
	/// <returns></returns>
	vector<Move> movable_cases(Side side) const;


	/// <summary>
	/// 디버그용 함수이다.
	/// Board에서의 기물의 위치를 8*8 격자판의 형태로 출력한다.
	/// <para>
	///		왼쪽 위가 a8, 오른쪽 아래가 h1이다.
	/// </para>
	/// </summary>
	void print(Move move = Move(Coord(), Coord())) const;
	/// <summary>
	/// 기물 수 판정의 더 편한 디버그를 위한 함수이다.
	/// <para> Board에서 어떤 움직임이 발생하는지 색을 입혀서 순서대로 보여준다. </para>
	/// <para> 백은 흰색, 흑은 회색으로 표시된다. </para>
	/// <para> 기물의 위치는와 움직일 위치는 진영에 따라 달리 표시된다(백:파랑,흑:빨강) </para>
	/// <para> 스페이스 키를 눌러 다음으로 넘어간다. </para>
	/// </summary>
	void print_movable_cases(Side side) const;
	/// <summary>
	/// 디버그용 함수이다.
	/// <para>
	///		print()를 이용해 직접 체스 게임을 하며 기능이 잘 구현되었는지 확인할 수 있도록 했다.
	/// </para>
	/// 
	/// <para>
	///		키의 이동은 WASD로, 좌표의 결정은 SPACE로 이뤄지며,
	///		Q는 취소, ESC는 종료이다.
	/// </para>
	/// <para>
	///		움직일 기물 선택과 움직일 좌표의 선택이 교대로,
	///		백과 흑의 차례가 교대로 이뤄진다.
	/// </para>
	/// <para>
	///		입력이 잘못되면 기물 선택부터 다시 해야한다.
	/// </para>
	/// </summary>
	static void GAME(string fen = default_fen);
	
private:
	/// <summary>
	/// 각각의 기물은 FEN 방식을 따라 기록된다.
	/// <para>
	///		기물 이름의 초성을 사용하며,
	///		백이 대문자, 흑이 소문자이다.
	/// </para>
	/// </summary>
	char board[64] = {};
	/// <summary>
	/// 차례를 나타낸다.
	/// <para>
	///		null값은 EMPTY 이다.
	/// </para>
	/// </summary>
	Side turn;
	/// <summary>
	/// 백의 킹사이드 캐슬링 가능 여부를 나타낸다.
	/// </summary>
	bool castling_K;
	/// <summary>
	/// 백의 퀸사이드 캐슬링 가능 여부를 나타낸다.
	/// </summary>
	bool castling_Q;
	/// <summary>
	/// 흑의 킹사이드 캐슬링 가능 여부를 나타낸다.
	/// </summary>
	bool castling_k;
	/// <summary>
	/// 흑의 퀸사이드 캐슬링 가능 여부를 나타낸다.
	/// </summary>
	bool castling_q;
	/// <summary>
	/// 앙파상 가능한 좌표를 나타낸다.
	/// <para>
	///		null 값은 Coord의 기본값인 {RANK_NON, FILE_NON} 이다.
	/// </para>
	/// </summary>
	Coord en_passant;
	/// <summary>
	/// 하프무브 값을 나타낸다.
	/// </summary>
	short half_move;
	/// <summary>
	/// 풀무브 값을 나타낸다.
	/// </summary>
	short full_move;


	/// <summary>
	/// 특정 좌표에 기물을 놓는 함수이다.
	/// </summary>
	/// <param name = 'coord'> 기물이 놓일 좌표 </param>
	/// <param name = 'piece'> 놓일 기물의 종류 </param>
	void set_piece(Coord coord, char piece);
	/// <summary>
	/// 특정 좌표에서 기물의 종류를 반환하는 함수이다.
	/// </summary>
	/// <param name="coord"> 기물의 종류를 확인할 좌표 </param>
	/// <returns> 좌표 위 기물의 종류를 반환 </returns>
	char get_piece(Coord coord) const;
	/// <summary>
	/// 특정 좌표에서 기물의 진영을 반환하는 함수이다.
	/// </summary>
	/// <param name="coord"> 기물의 진영을 확인할 좌표 </param>
	/// <returns> 좌표 위 기물의 진영을 반환(enum Side 참고) </returns>
	inline Side get_side(Coord coord) const;


	/// <summary>
	/// 좌표 하나와 진영을 받아서 해당 위치에 말을 놓을 수 있는지를 판별하는 함수이다.
	/// <para>
	///		기물을 움직이기 이전에 그것이 가능한지를 판별한다.
	/// </para>
	/// </summary>
	/// <param name="dst_crd"> 확인을 원하는 좌표 </param>
	/// <param name="side"> 진영 </param>
	/// <returns> 좌표가 비여있거나 적의 기물이 있으면 참, 아니면 거짓 </returns>
	inline bool movable_piece(const Coord& dst_crd, Side side) const;
	/// <summary>
	/// movable_to()가 true임을 전제로 기물의 움직임이 실현된 결과를 반환한다
	/// <para>
	///		위의 movable_to()가 true임을 전제로 사용한다.
	/// </para>
	/// </summary>
	/// <param name="src_crd"> 원래 기물의 위치 </param>
	/// <param name="dst_crd"> 목표 기물의 위치 </param>
	/// <returns> Board type의 객체 </returns>
	inline void move_piece_to(Coord src_crd, Coord dst_crd);
	/// <summary>
	///	move라는 수를 두는 함수이다.
	/// </summary>
	/// <param name="move"> 기물의 움직임 </param>
	void apply_move(Move move);


	/// <summary>
	/// 킹의 좌표를 받아 체크 상황인지 확인하는 함수이다.
	/// <para>
	///		킹의 진영은 상관 없다.
	/// </para>
	/// </summary>
	/// <param name="k_coord"> 킹의 좌표 </param>
	/// <returns> 좌표 위 킹이 체크 상황이면 참, 아니면 거짓 </returns>
	inline bool check(Coord k_crd, Side side) const;
	/// <summary>
	/// 현재 Board가 체크 상황인지 확인하는 함수이다.
	/// <para>
	///		확인하는 과정에서 movable()을 사용하므로, apply_move()에서 참조하도록 한다.
	/// </para>
	/// </summary>
	/// <returns> 체크 상태에 놓인 진영을 반환 </returns>
	inline Side check() const;
	/// <summary>
	/// 진영을 받아 해당 진영이 체크메이트 상황인지를 확인하는 함수이다.
	/// </summary>
	/// <returns> 진영이 체크메이트면 참, 아니면 거짓 </returns>
	inline bool checkmate(Side side) const;


	inline vector<Move> __movable_k(Coord src_crd, Side side) const;
	/// <summary>
	/// 왕이 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 왕의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Move> movable_k(Coord src_crd, Side side) const;
	/// <summary>
	/// 퀸이 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 퀸의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Move> movable_q(Coord src_crd, Side side) const;
	/// <summary>
	/// 룩이 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 룩의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Move> movable_r(Coord src_crd, Side side) const;
	/// <summary>
	/// 비숍이 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 비숍의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Move> movable_b(Coord src_crd, Side side) const;
	/// <summary>
	/// 나이트가 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 나이트의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Move> movable_n(Coord src_crd, Side side) const;
	/// <summary>
	/// 폰이 움직일 수 있는 모든 좌표를 계산해 반환한다.
	/// </summary>
	/// <param name="src_crd"> 현재 폰의 좌표 </param>
	/// <param name="is_white_side"> 진영 </param>
	/// <returns> 가능한 움직임의 목록 </returns>
	inline vector<Move> movable_p(Coord src_crd, Side side) const;

};

#endif // !BOARD_H_INCLUDED