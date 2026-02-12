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

        Dialogue_01 = 0,    // 체셔: 오호! 정말로 여왕을...
        Dialogue_02,        // 앨리스: 뭐라고? 여왕 같은 건...
        Dialogue_03,        // 체셔: 그래? 그렇다면...
        Dialogue_04,        // 앨리스: 규칙이라고?
        Dialogue_05,        // 체셔: 그래, 여왕 폐하...
        Dialogue_06,        // 체셔: 지금 보드랜드의...
        Dialogue_07,        // 앨리스: 어떻게 해야 하는데?
        Dialogue_08,        // 체셔: 자, 눈을 감아 봐.
        Dialogue_09,        // 체셔: 돌아가서도 잊지 말라고.

        ScreenDarken,       // 2. 화면 암전 (연출 -> 자동 넘김)
        DarkCatDialogue,
        Placement,          // 3. 배치 (즉시)
        ShowPopup,          // 4. 팝업 (입력 대기)

        AliceWalk,
        TileShake,          // 타일 흔들림
        TileFall,           // 타일 추락
                  
        PreCreditDarken,
        Credit_Part1,       // 6. 크레딧1 (입력 대기)
        Credit_Part2,       // 7. 크레딧2 (입력 대기)
        EndingCredit,        // 8. 엔딩 (입력 시 타이틀)
        ReturnToMain
    };

    enum class FadeState
    {
        None,
        FadeIn,     // 나타나는 중
        Idle,       // 다 나와서 대기 중
        FadeOut     // 사라지는 중
    };

    void Start() override;
    void Update(float deltaTime) override;

    void NextStep(bool force = false);

private:
    void SetUIAlpha(GameObject* target, float alpha);
    bool IsCreditStep(EndStep step);

    GameObject* m_aliceDialogueUI = nullptr;
    Text* m_aliceDialogueText = nullptr;

    GameObject* m_chesherDialogueUI = nullptr;
    Text* m_chesherDialogueText = nullptr;

    GameObject* m_popupUI = nullptr;
    GameObject* m_credit1UI = nullptr;
    GameObject* m_credit2UI = nullptr;
    GameObject* m_finalCreditUI = nullptr;

    GameObject* m_additionalFall1 = nullptr;
    GameObject* m_additionalFall2 = nullptr;


    CombatController* m_combatController = nullptr;

    EndStep m_currentStep = EndStep::None;

    FadeState m_fadeState = FadeState::None;
    float m_fadeTimer = 0.0f;
    const float FADE_DURATION = 1.0f;

    bool m_canProceedToNextStep = true;

    float m_stateTimer = 0.0f;

    bool m_isEndingSequenceStarted = false;

    void PrepareTileEffect();

    std::wstring m_targetPopupText;    
    bool m_isTyping = false;           
    float m_typingTimer = 0.0f;        
    int m_typingIndex = 0;             
    const float TYPING_SPEED = 0.3f;

    struct TileInfo {
        GameObject* obj;
        Vector3 originalPos;
    };
    std::vector<TileInfo> m_targetTiles; 

    GameObject* m_aliceObj = nullptr;

    float m_shakeIntensity = 0.1f;       
    float m_fallSpeed = 0.0f;            
};