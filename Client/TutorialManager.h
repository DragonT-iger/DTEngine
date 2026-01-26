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

    void NextStep();

private:
    GameObject* leftChat = nullptr;
    GameObject* rightChat = nullptr;

    Text* leftChatText = nullptr; // awake에서 자동으로 할당
    Text* rightChatText = nullptr;

    GameObject* m_QueenUI = nullptr;
    GameObject* m_CatUI = nullptr;

    int m_CurrentStepIndex = -1;
};