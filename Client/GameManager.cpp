#include "GameManager.h"
#include "Text.h"
#include "GameObject.h"

BEGINPROPERTY(GameManager)
//DTPROPERTY(GameManager, m_leftHealth) static inline은 직렬화 시 의미가 없어짐 값이 덮힘 주의할것
DTPROPERTY(GameManager, m_money)
DTPROPERTY(GameManager, m_life)
ENDPROPERTY()


void GameManager::Awake() {
	
	//if (curCost) curCost->SetText(L"asdasd");
}

void GameManager::SetLife(int life)
{
}
