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

	curMoney = 30; // 임시.
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
	if (curStageLevel < 6) {
		curStageLevel++;
	}
}

void GameManager::Update(float deltaTime)
{
	//if(InputManager::Instance().GetKeyDown(KeyCode::L)) 
	//{
	//	SetLife(GetLife() - 1);
	//}

	// 여기서 bool 가져와서 성공 실패 나오는 경우 setdelay true
	//if (isDelay)
	//{
	//		delayTime += deltaTime;
	//}

	//if (delayTime >= 1)
	//{
	//		// 초기화 시켜주고 window setactive true 처리하는 함수.
	//		isDelay = false;	
	//		delayTime = 0;
	//}
	
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

//void GameManager::ResetBattleResultUI()
//{
//		if (!m_victoryWindow || !m_loseWindow)
//				return;
//
//		// 초기화 시 결과창 항상 닫혀있는걸로..
//		m_victoryWindow->SetActive(false);
//		m_loseWindow->SetActive(false);
//
//		m_battleResultState = BattleResultState::None;
//		ApplyResultInteractionLock(false);
//}
//
//void GameManager::ShowVictoryWindow()
//{
//		if (!m_victoryWindow)
//		{
//				std::cout << "victoryWindow 없음" << std::endl;
//				return;
//		}
//		if (!m_loseWindow)
//		{
//				std::cout << "losewindow 없음" << std::endl;
//				return;
//		}
//		
//	
//		m_loseWindow->SetActive(false);
//		m_victoryWindow->SetActive(true);
//		ApplyResultInteractionLock(true);
//
//		m_battleResultState = BattleResultState::Victory;
//}
//
//void GameManager::ShowLoseWindow()
//{
//		if (!m_victoryWindow || !m_loseWindow)
//				return;
//
//		m_victoryWindow->SetActive(false);
//		m_loseWindow->SetActive(true);
//		ApplyResultInteractionLock(true);
//
//		m_battleResultState = BattleResultState::Failed;
//}

//void GameManager::LoadNextStageFromCurrent()
//{
//		if (m_useSingleSceneStageFlow)
//		{
//				Scene* activeScene = SceneManager::Instance().GetActiveScene();
//				if (!activeScene) return;
//
//				TilemapData* stageMaps[5] = { m_stageTilemap0, m_stageTilemap1, m_stageTilemap2, m_stageTilemap3, m_stageTilemap4 };
//				constexpr int STAGE_COUNT = 5;
//
//				int nextIndex = m_currentStageIndex + 1;
//				if (nextIndex >= STAGE_COUNT || !stageMaps[nextIndex])
//				{
//						ClientSceneManager::Instance().LoadScene("EndingScene");
//						return;
//				}
//
//				if (!m_stageTilemapGenerator || !m_stageBattleGrid)
//				{
//						std::cout << "tilemapgenerator, battlegrid 빠짐. gamemanager한테 넣어줘야함." << std::endl;
//						return;
//				}
//
//				m_currentStageIndex = nextIndex;
//
//				// 씬을 바꾸지 않고 tilemapdata만 교체 후 타일/적 생성 재구축.
//				m_stageTilemapGenerator->SetMapData(stageMaps[m_currentStageIndex]);
//				m_stageTilemapGenerator->RebuildFromCurrentData();
//				m_stageBattleGrid->SetTilemapGenerator(m_stageTilemapGenerator);
//				ResetBattleResultUI();
//
//				// 단일  씬에서도 스테이지 레벨/힌트/cost UI 정책을 동일하게 적용.
//				HandleSceneInit(activeScene->GetName());
//				return;
//		}
//}
//void GameManager::ReloadCurrentStage()
//{
//
//		Scene* activeScene = SceneManager::Instance().GetActiveScene();
//		if (!activeScene) return;
//
//		ClientSceneManager::Instance().LoadScene(activeScene->GetName());
//}

//void GameManager::HandleSceneInit(const std::string& sceneName)
//{
//		// 씬/스테이지 초기화 시 슬라이더 핸들을 저장된 오디오 값으로 동기화.
//		if (m_bgmSlider) m_bgmSlider->SetValue(curbgmValue);
//		if (m_sfxSlider) m_sfxSlider->SetValue(cursfxValue);
//
//		if (sceneName == "TitleScene")
//		{
//				curStageLevel = 1;
//				usedStageMoney = 0;
//				stageString = { false, false, false, false, false, false };
//				// 스킬횟수는 "현재치 + 복구기준치" 2개만 사용.
//				// 타이틀 복귀 시에도 보상으로 올라간 default는 유지하고 현재치만 default로 동기화한다.
//				healSkillCount = defaultHealSkillCount;
//				attackSkillCount = defaultAttackSkillCount;
//				ApplyResultInteractionLock(false);
//				if (m_stageLevelText) m_stageLevelText->SetText(L"STAGE 1");
//				if (m_stageDialogueText) m_stageDialogueText->SetText(L"체셔의 규칙이 다시 시작된다.");
//				return;
//		}
//
//		// 단일 씬에서 진행하므로 씬 이름과 무관하게 현재 스테이지 인덱스를 기준으로 UI를 맞춘다.
//		curStageLevel = m_currentStageIndex + 1;
//
//		// 스테이지 진입 시 "보유 총 코스트" 기준 리셋.
//		usedStageMoney = 0;
//		curMoney = totalAcquiredMoney;
//		healSkillCount = defaultHealSkillCount;
//		attackSkillCount = defaultAttackSkillCount;
//		if (m_money) m_money->SetText(std::to_wstring(curMoney));
//
//		if (m_stageLevelText)
//		{
//				m_stageLevelText->SetText(L"STAGE " + std::to_wstring(curStageLevel));
//		}
//
//		if (m_stageDialogueText)
//		{
//				// 스테이지별 힌트는 1회만 노출한다.
//				if (curStageLevel >= 1 && curStageLevel <= 5 && !stageString[curStageLevel])
//				{
//						if (curStageLevel == 1) m_stageDialogueText->SetText(L"규칙을 잘 적용해봐.");
//						else if (curStageLevel == 2) m_stageDialogueText->SetText(L"방어 타일 활용이 핵심이야.");
//						else if (curStageLevel == 3) m_stageDialogueText->SetText(L"함정 타일 타이밍을 노려.");
//						else if (curStageLevel == 4) m_stageDialogueText->SetText(L"우선순위 공격 규칙을 점검해.");
//						else if (curStageLevel == 5) m_stageDialogueText->SetText(L"보스를 먼저 끊는 전략을 써.");
//
//						stageString[curStageLevel] = true;
//				}
//				else
//				{
//						m_stageDialogueText->SetText(L"");
//				}
//		}
//
//		// 스테이지(다음/재도전) 진입 시 StartButton만 초기 상태로 복구.
//		if (m_startButtonEvent) m_startButtonEvent->ResetForStage();
//}

//void GameManager::RegisterRuntimeAlly(GameObject* allyObj)
//{
//		if (!allyObj) return;
//
//		// 중복으로 등록은 아니게.
//		auto it = std::find(m_runtimeAllies.begin(), m_runtimeAllies.end(), allyObj);
//		if (it == m_runtimeAllies.end())
//		{
//				m_runtimeAllies.push_back(allyObj);
//		}
//}

//void GameManager::ClearRuntimeUnitCaches()
//{
//		// vector 초기화.
//		m_runtimeAllies.clear();
//}

//void GameManager::ApplyResultInteractionLock(bool lock)
//{
//		m_isResultInteractionLocked = lock;
//
//		// 설정창 켜졌으면 꺼버림. 
//		if (lock && m_settingWindow) m_settingWindow->SetActive(false);
//
//		// 레이 상호작용 차단/복구.
//		if (m_rayObj)
//		{
//				RayCastHitEvent* ray = m_rayObj->GetComponent<RayCastHitEvent>();
//				if (ray)
//				{
//						ray->SetRay(!lock);
//						ray->CloseAllWindows();
//				}
//				m_rayObj->SetActive(!lock);
//		}
//
//		// 결과창 외 입력 버튼/오브젝트를 묶어서 on/off.
//		GameObject* blocks[5] = { m_blockInputObj0, m_blockInputObj1, m_blockInputObj2, m_blockInputObj3, m_blockInputObj4 };
//		for (GameObject* obj : blocks)
//		{
//				if (obj) obj->SetActive(!lock);
//		}
//}
//
//void GameManager::EnsureResultWindowRefs()
//{
//		if (!m_victoryWindow)
//		{
//				m_victoryWindow = GameObject::Find("VictoryWindow");
//				if (!m_victoryWindow) m_victoryWindow = GameObject::Find("VictoryWindowBG");
//		}
//
//		if (!m_loseWindow)
//		{
//				m_loseWindow = GameObject::Find("LoseWindow");
//				if (!m_loseWindow) m_loseWindow = GameObject::Find("LoseWindowBG");
//		}
//}
//
//void GameManager::ClearStageUnits()
//{
//		Scene* activeScene = SceneManager::Instance().GetActiveScene();
//		if (!activeScene) return;
//
//		std::vector<GameObject*> toDestroy;
//
//		// allyobj 넣어주기.
//		for (GameObject* allyObj : m_runtimeAllies)
//		{
//				if (allyObj) toDestroy.push_back(allyObj);
//		}
//
//		// tilemapGenerator에 적군 유닛 가져오기.
//		if (m_stageTilemapGenerator)
//		{
//				auto& spawnedEnemies = m_stageTilemapGenerator->GetSpawnedEnemys();
//				for (GameObject* enemyObj : spawnedEnemies)
//				{
//						if (enemyObj) toDestroy.push_back(enemyObj);
//				}
//		}
//
//		// 혹시라도 없으면 찾기. 이건 근데 안할거임. 필요하면 추가할까 생각중.
//		/*const auto& objects = activeScene->GetGameObjects();
//		for (const auto& objPtr : objects)
//		{
//				GameObject* obj = objPtr.get();
//				if (!obj) continue;
//
//				if (obj->GetComponent<AllyUnit>() || obj->GetComponent<EnemyUnit>())
//				{
//						if (std::find(toDestroy.begin(), toDestroy.end(), obj) == toDestroy.end())
//						{
//								toDestroy.push_back(obj);
//						}
//				}
//		}*/
//
//		// 아군 적군 둘 다 삭제. 
//		for (GameObject* obj : toDestroy)
//		{
//				activeScene->Destroy(obj);
//		}
//
//		// 이번 스테이지에서 추적하던 런타임 캐시는 모두 비운다.
//		ClearRuntimeUnitCaches();
//}
