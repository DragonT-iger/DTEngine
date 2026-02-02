#pragma once
#include "MonoBehaviour.h"
#include <vector>
#include <string>

class GameObject;
class Text; 
class CombatController;
class Prefab;
class TilemapGenerator;

class TutorialManager : public MonoBehaviour
{
    DTGENERATED_BODY(TutorialManager);

public:
    void Awake() override;
    void Start() override;

    void Update(float deltaTime) override;

    void NextStep();

    bool GetRayActive() { return m_rayActive; }
    void SetRayActive(bool active) { m_rayActive = active; }

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

    int m_CurrentStepIndex = -1;
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
    
    TilemapGenerator* m_tilemapGenerator = nullptr;

    float m_circleCenterX = 0.0f;
    float m_circleCenterY = 0.0f;
};