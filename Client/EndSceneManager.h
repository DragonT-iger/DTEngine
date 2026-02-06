#pragma once
#include "MonoBehaviour.h"

class GameObject;
class CombatController;
class Text;

class EndSceneManager : public MonoBehaviour
{
    DTGENERATED_BODY(EndSceneManager);

public:
    enum class EndStep
    {
        None = -1,
        Dialogue = 0,       // 1. 대화 (입력 대기)
        ScreenDarken,       // 2. 화면 암전 (연출 -> 자동 넘김)
        DarkCatDialogue,
        Placement,          // 3. 배치 (즉시)
        ShowPopup,          // 4. 팝업 (입력 대기)
        AliceWalk,          // 5. 걷기 (연출 -> 자동 넘김 or 입력)
        Credit_Part1,       // 6. 크레딧1 (입력 대기)
        Credit_Part2,       // 7. 크레딧2 (입력 대기)
        EndingCredit        // 8. 엔딩 (입력 시 타이틀)
    };

    void Start() override;
    void Update(float deltaTime) override;

    void NextStep(bool force = false);

private:
    GameObject* m_dialogueUI = nullptr;
    Text* m_dialogueText = nullptr;
    GameObject* m_popupUI = nullptr;
    GameObject* m_credit1UI = nullptr;
    GameObject* m_credit2UI = nullptr;
    GameObject* m_finalCreditUI = nullptr;

    CombatController* m_combatController = nullptr;

    EndStep m_currentStep = EndStep::None;

    bool m_canProceedToNextStep = true;

    float m_stateTimer = 0.0f;
};