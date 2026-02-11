#pragma once

#include "MonoSingleton.h"
#include "../DTEngine/SoundManager.h"
#include <string>
#include <array>
#include <vector>

class Text;
class GameObject;
class TilemapData;
class TilemapGenerator;
class BattleGrid;
class UISlider;
class ClickStartButton;
class ChesherGuideEvent;

//enum class BattleResultState
//{
//		None,
//		Victory,
//		Failed
//};

//우리 엔진의 MonoSingleton은 잘 생각해봐야 하는게 c++ 코드 상에서의 싱글톤이지
//엔진 컴포넌트 상에서는 싱글톤 패턴이 아님. 각자 씬마다의 고유한 인스턴스를 가진다는 뜻임.
//그러므로 static 변수를 사용하지 않는다면 여러 씬에서 공유되는 변수를 만들수 없음.
//즉 컴포넌트 각각의 변수들이 존재할 뿐이지 전역적인 변수가 아니라는 뜻
//DontDestroyOnLoad 개념이 존재하지 않음. 주의해서 사용할 것
//GameManager는 전역적인 여러 상태들을 저장함 ex) 남은 목숨, 점수 등등
class GameManager : public MonoSingleton<GameManager>
{
	DTGENERATED_BODY(GameManager)

public:

	void Start() override;
	void SetLife(int life);
	int GetLife() const;
	void ResetLifeAndMoney();
	void SetMoney(int money);
	int GetMoney() const;
	int GetTotalAcquiredMoney() const { return totalAcquiredMoney; }
	int GetUsedStageMoney() const { return usedStageMoney; }


	void Update(float deltaTime) override;
	
	void SetBgmValue(float value) { curbgmValue = value; SoundManager::Instance().SetBGMVolume(value); }
	float GetBgmValue() const { return curbgmValue; }

	void SetSfxValue(float value) { cursfxValue = value; SoundManager::Instance().SetSFXVolume(value);}
	float GetSfxValue() const { return cursfxValue; }

	void SetTimeScale(int scale);
	int GetTimeScale() const { return curTimeScale; }
	int GetPrevTimeScale() const { return prevTimeScale; }

	int GetStageLevel() const { return curStageLevel; }
	void SetStageLevel(int stage) { curStageLevel = stage; }


	int GetHealSkillcount() const { return healSkillCount; }
	void SetHealSkillCount(int count) { healSkillCount = count; }
	int GetMaxHealSkillCount() const { return defaultHealSkillCount; }

	int GetAttackSkillcount() const { return attackSkillCount; }
	void SetAttackSkillCount(int count) { attackSkillCount = count; }
	int GetMaxAttackSkillCount() const { return defaultAttackSkillCount; }

	//bool GetDelay() const { return isDelay; }
	//void SetDelay(bool value) { isDelay = value; }

	// 보상으로 스킬 최대/현재 횟수를 함께 증가시킨다.
	// - 현재 사용 가능 횟수(healSkillCount/attackSkillCount)
	// - 복구 기준 최대치(defaultHealSkillCount/defaultAttackSkillCount)
	void GrantHealSkillReward(int amount = 1);
	void GrantAttackSkillReward(int amount = 1);

	void NextStage();

	// 전투 결과(UI) 처리 전담.
	//void ResetBattleResultUI();
	//void ShowVictoryWindow();
	//void ShowLoseWindow();
	//bool IsVictory() const { return m_battleResultState == BattleResultState::Victory; }
	//bool IsFailed() const { return m_battleResultState == BattleResultState::Failed; }
	//bool IsResultInteractionLocked() const { return m_isResultInteractionLocked; }

	//// 스테이지 이동 API.
	//// 결과창 버튼(다음/재시작) 함수.
	//void LoadNextStageFromCurrent();
	//void ReloadCurrentStage();

	//// Scene 진입 전/후 초기화 훅.
	//void HandleSceneInit(const std::string& sceneName);

	//// 아군 생성 유닛 등록용.
	//void RegisterRuntimeAlly(GameObject* allyObj);
	//void ClearRuntimeUnitCaches();

private:
	
	Text* m_money = nullptr;
	Text* m_life  = nullptr;

	UISlider* m_bgmSlider = nullptr;
	UISlider* m_sfxSlider = nullptr;

	static inline int maxLife = 9;
	static inline int curLife = 9;

	static inline int curMoney = 30;						   	// 사용하는 돈임.
	static inline int totalAcquiredMoney = 30;			// 이게 초기화때 쓸 돈임.
	static inline int usedStageMoney = 0;

	static inline int curStageLevel = 1;	// 스테이지 레벨.

	static inline float curbgmValue = 0.5f;		
	static inline float cursfxValue = 0.5f;
	
	static inline int curTimeScale = 1;		// 배속값 위해서.
	static inline int prevTimeScale = 1.0f;

	// 스킬은 처음에 무조건 0. 이후에 count 증가함.
	static inline int healSkillCount = 0;
	static inline int attackSkillCount = 0;
	static inline int defaultHealSkillCount = 0;
	static inline int defaultAttackSkillCount = 0;

	// delay를 위해서
	//static inline bool isDelay = false;
	//static inline int delayTime = 0;

	// 스테이지 string은 각 스테이지에서 1회만.
	//static inline std::array<bool, 6> stageString = { false, false, false, false, false, false };


	// 승패 결과창이 열렸을 때 다른 상호작용을 막기 위한 참조들.
	//GameObject* m_settingWindow = nullptr;
	//GameObject* m_rayObj = nullptr;
	//GameObject* m_blockInputObj0 = nullptr;
	//GameObject* m_blockInputObj1 = nullptr;
	//GameObject* m_blockInputObj2 = nullptr;
	//GameObject* m_blockInputObj3 = nullptr;
	//GameObject* m_blockInputObj4 = nullptr;
	//bool m_isResultInteractionLocked = false;

	// 승리 패배로 처리해야하니까. 
	//GameObject* m_victoryWindow = nullptr;
	//GameObject* m_loseWindow = nullptr;
	//BattleResultState m_battleResultState = BattleResultState::None;

	// 단일 씬 + 타일맵 교체 방식
	//bool m_useSingleSceneStageFlow = true;

	// 레거시 호환용
	//std::string m_singleStageSceneName = "Stage1";
	//TilemapData* m_stageTilemap0 = nullptr;
	//TilemapData* m_stageTilemap1 = nullptr;
	//TilemapData* m_stageTilemap2 = nullptr;
	//TilemapData* m_stageTilemap3 = nullptr;
	//TilemapData* m_stageTilemap4 = nullptr;
	//int m_currentStageIndex = 0;
	//TilemapGenerator* m_stageTilemapGenerator = nullptr;
	//BattleGrid* m_stageBattleGrid = nullptr;
	//Text* m_stageLevelText = nullptr;
	//Text* m_stageDialogueText = nullptr;

	//ClickStartButton* m_startButtonEvent = nullptr;

	//void ApplyResultInteractionLock(bool lock);
	//void EnsureResultWindowRefs();		// 결과창 못찾으면 find로 찾기. 
	//void ClearStageUnits();						// stage unit 포인터 비워주기.
	//std::vector<GameObject*> m_runtimeAllies;
};

