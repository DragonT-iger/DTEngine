#include "CommonInclude.h"
#include "../Engine/Game.h"

int main() {

	// ���� üũ
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);



	Game game;

	if (!game.Init()) {
		std::cout << "���� �ʱ�ȭ ����" << std::endl;
		return false;
	}

	game.Run();

	game.Release();




}