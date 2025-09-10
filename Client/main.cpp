#include "CommonInclude.h"
#include "../Engine/Game.h"

int main() {

	// 누수 체크
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);



	Game game;

	if (!game.Init()) {
		std::cout << "게임 초기화 실패" << std::endl;
		return false;
	}

	game.Run();

	game.Release();




}