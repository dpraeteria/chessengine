#include "pruning_tree.h" //사용하지는 않지만 디버깅 과정에 포함시키기 위함
#include "Board.h"

int main(void) {
	/*
		< how to play CHESS >
	
	키의 이동 : WASD
	좌표 결정 : SPACE로 이뤄지며,
	취소 : Q
	종료 : ESC
	
	백과 흑의 차례가 교대로 이뤄지며,
		그 안에서는 기물 선택과 목표 좌표의 선택이 교대로 이뤄진다.
	
	입력이 잘못되면 기물 선택부터 다시 시작된다.
	*/
	Board::GAME();

	return 0;
}