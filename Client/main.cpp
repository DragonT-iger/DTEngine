#include "Common.h"
#include "Game.h"

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//_CrtSetBreakAlloc(380);
	Game game;
	//_CrtSetBreakAlloc(2190250);

	if (!game.Initialize()) {
		std::cout << "게임 초기화 실패" << std::endl;
		return false;
	}

	game.Run();

	game.Release();

	return 0;
}