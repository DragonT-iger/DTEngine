#pragma once
#include "MonoBehaviour.h"
#include <vector>
#include <string>

class GameObject;
class Text; 

class TutorialManager : public MonoBehaviour
{
    DTGENERATED_BODY(TutorialManager);

public:
    void Awake() override;
    void Start() override;

    void Update(float deltaTime) override;

    void NextStep();

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

	GameObject* m_BattleStartCat = nullptr;

    int m_CurrentStepIndex = -1;
};