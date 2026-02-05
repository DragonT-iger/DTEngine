#include "GameManager.h"
#include "Text.h"
#include "GameObject.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"

BEGINPROPERTY(GameManager)
//DTPROPERTY(GameManager, m_leftHealth) static inline은 직렬화 시 의미가 없어짐 값이 덮힘 주의할것
//DTPROPERTY(GameManager, m_money)
//DTPROPERTY(GameManager, m_life)
ENDPROPERTY()


void GameManager::Start() {
	
	if (m_money) m_money->SetText(std::to_wstring(curMoney));
	if (m_life) m_life->SetText(std::to_wstring(curLife));
}

void GameManager::SetLife(int life)
{
	if (m_life) m_life->SetText(std::to_wstring(life));
	curLife = life;
}

int GameManager::GetLife() const
{
	return curLife;
}

void GameManager::SetMoney(int money)
{
	if (m_money) m_money->SetText(std::to_wstring(money));
	curMoney = money;
}

int GameManager::GetMoney() const
{
	return curMoney;
}

void GameManager::Update(float deltaTime)
{
	if(InputManager::Instance().GetKeyDown(KeyCode::L)) {
		SetLife(GetLife() - 1);
	}
}

void GameManager::SetTimeScale(int scale)
{
		if (curTimeScale != 0.0f)
		{
				prevTimeScale = curTimeScale;
		}

		curTimeScale = scale;
		SceneManager::Instance().GetActiveScene()->SetTimeScale(scale);
}
