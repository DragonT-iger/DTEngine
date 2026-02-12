#include "GameManager.h"
#include "Text.h"
#include "GameObject.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ClientSceneManager.h"
#include "TilemapGenerator.h"
#include "TilemapData.h"
#include "BattleGrid.h"
#include "RayCastHitEvent.h"
#include "UISlider.h"
#include "ClickStartButton.h"
#include "ChesherGuideEvent.h"
#include <vector>


BEGINPROPERTY(GameManager)
//DTPROPERTY(GameManager, m_leftHealth) static inline은 직렬화 시 의미가 없어짐 값이 덮힘 주의할것
DTPROPERTY(GameManager, m_money)
DTPROPERTY(GameManager, m_life)
//DTPROPERTY(GameManager, m_victoryWindow)
//DTPROPERTY(GameManager, m_loseWindow)
//DTPROPERTY(GameManager, m_stageTilemapGenerator)
//DTPROPERTY(GameManager, m_stageBattleGrid)
//DTPROPERTY(GameManager, m_stageLevelText)
//DTPROPERTY(GameManager, m_stageDialogueText)
DTPROPERTY(GameManager, m_bgmSlider)
DTPROPERTY(GameManager, m_sfxSlider)
//DTPROPERTY(GameManager, m_startButtonEvent)
//DTPROPERTY(GameManager, m_settingWindow)
//DTPROPERTY(GameManager, m_rayObj)
ENDPROPERTY()


void GameManager::Start() {
	
	if (m_money) m_money->SetText(std::to_wstring(curMoney));
	if (m_life) m_life->SetText(std::to_wstring(curLife));

	// 설정창 슬라이더가 GameManager 저장값과 동일한 위치를 가지도록 보정.
	if (m_bgmSlider) m_bgmSlider->SetValue(curbgmValue);
	if (m_sfxSlider) m_sfxSlider->SetValue(cursfxValue);

	SetMoney(30);// curMoney = 30; // 임시.
	healSkillCount = defaultHealSkillCount;
	attackSkillCount = defaultAttackSkillCount;
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

void GameManager::ResetLifeAndMoney()
{
		maxLife = 9;
		curLife = 9;

		curMoney = 30;
		totalAcquiredMoney = 30;

		healSkillCount = 0;
		attackSkillCount = 0;
		defaultHealSkillCount = 0;
		defaultAttackSkillCount = 0;
}

void GameManager::SetMoney(int money)
{
		int delta = money - curMoney;

		if (delta > 0) totalAcquiredMoney += delta;   // 획득분 누적
		if (delta < 0) usedStageMoney += -delta;      // 사용분 누적

	if (m_money) m_money->SetText(std::to_wstring(money));
	curMoney = money;
}

int GameManager::GetMoney() const
{
	return curMoney;
}

void GameManager::GrantHealSkillReward(int amount)
{
		if (amount <= 0) return;

		// 보상은 둘 다 올려주기. 
		defaultHealSkillCount += amount;
		healSkillCount += amount;
}

void GameManager::GrantAttackSkillReward(int amount)
{
		if (amount <= 0) return;

		// 보상은 둘 다 올려주기. 
		defaultAttackSkillCount += amount;
		attackSkillCount += amount;
}

void GameManager::NextStage()
{
	curStageLevel++;
}

void GameManager::Update(float deltaTime)
{
	//std::cout << "bgm " << curbgmValue << " " << cursfxValue << std::endl;
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

