#pragma once
#include "MonoBehaviour.h"
#include <vector>
#include <string>

class GameObject;
class Text; 
class CombatController;
class Prefab;
class TilemapGenerator;
class UIButton;
class RayCastHitEvent;

class TutorialManager : public MonoBehaviour
{
    DTGENERATED_BODY(TutorialManager);
public:

    enum class TutorialStep
    {
        None = -1,
        Intro_AliceQuestion = 0,        // "여긴 어디지?"
        Intro_WorldExplanation,         // "이곳은 규칙이 지배하는..."
        Setup_CombatUnit,               // 유닛 배치 및 초기화
        Vignette_FadeIn,                // 비네트 효과 시작 (화면 어두워짐)
        Vignette_FadeOut_CatAppear,     // 비네트 해제 및 고양이 등장 ("너만의 규칙을...")
        Alice_AskRule,                  // "규칙을 어떻게 정해야 하지?"
        Cat_OfferHelp,                  // "내가 한번 도와줘 볼까?"
        Cat_HideChat,                   // 채팅창 숨김 (전환)
        Cat_Explain_NeedSoldier,        // "체스 병사의 도움이 필요해"
        Cat_Highlight_Tiles,            // "빛나는 타일을 눌러봐" (원형 마스크)
        Cat_Explain_Cost,               // "코스트를 지불하고 소환" (카메라 이동)
        Cat_Order_Summon,               // (바로 넘어감)
        Cat_Explain_Control,            // "이동과 전투 규칙을 정해줄 수 있어"
        Cat_Explain_Wait,               // "대기 규칙을 적용하면..."
        Cat_Strategy_Closest,           // "가까운 놈을 노려 보지"
        Cat_Order_Confirm,              // "확인을 눌러"
        Cat_Explain_EnemyPath,          // "적은 지정된 경로를..."
        Cat_Explain_EnemyIntent,        // "적을 누르면..."
        Cat_Explain_EnemyPathWait,
        Cat_Strategy_DefenseTile,       // "방어 타일 위에 있으면..."
        Cat_Order_StartBattle,          // "싸움을 붙여 보자고!"
        Cat_Explain_EnemyRange,         // "토끼 녀석들은 공격 범위가 정해져 있어" 
        Cat_Explain_EnemyRange_End,     // "너를 위해서 말이야" 
        Battle_InProgress,              // (Case 22: 현재 주석처리됨, 순서 유지용)
        Victory                         // 승리 UI (Case 23)
    };

    void Awake() override;
    void Start() override;

    void Update(float deltaTime) override;

    void NextStep(bool force = false);

    bool GetRayActive() { return m_rayActive; }
    void SetRayActive(bool active) { m_rayActive = active; }

    TutorialStep GetCurrentStep() { return m_CurrentStep; }

private:
    GameObject* leftChat = nullptr;
    GameObject* rightChat = nullptr;

    Text* leftChatText = nullptr; // awake에서 자동으로 할당
    Text* rightChatText = nullptr;

    GameObject* m_queenUI = nullptr;
    GameObject* m_catUI = nullptr;

	GameObject* m_catUI2 = nullptr;
	Text* m_catText2 = nullptr;

	GameObject* m_firstNextButton = nullptr;
	GameObject* m_secondNextButton = nullptr;

	GameObject* m_BattleStart = nullptr;

    GameObject* m_victoryUI = nullptr;

    TutorialStep m_CurrentStep = TutorialStep::None;
	bool m_canProceedToNextStep = true;

    GameObject* m_chessSoldier = nullptr;
    CombatController* m_combatController = nullptr;
    GameObject* m_aliceGameObject = nullptr;

    GameObject* m_infoUI = nullptr;
    Prefab* m_glowTilePrefab = nullptr;

    bool m_rayActive = false;
    bool m_aliceDead = false;

    bool m_isVignetteSequence = false;
    float m_currentSoftness = 1.0f;
    float m_vignetteDelayTimer = 0.0f;


    float m_circleRadius = 1.0f;
    
    TilemapGenerator* m_tilemapGenerator = nullptr;

    UIButton* m_RRSokayButton = nullptr;

    Prefab* m_tutorialAdditionalEnemyPrefab = nullptr;
    GameObject* m_tutorialAdditionalEnemy = nullptr;

    RayCastHitEvent* m_rayCastHitEvent = nullptr;

};