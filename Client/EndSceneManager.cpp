#include "pch.h"
#include "EndSceneManager.h"
#include "GameObject.h"
#include "InputManager.h"
#include "CombatController.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "ClientSceneManager.h" 
#include "Text.h"

BEGINPROPERTY(EndSceneManager)
DTPROPERTY(EndSceneManager, m_dialogueUI)
DTPROPERTY(EndSceneManager, m_popupUI)
DTPROPERTY(EndSceneManager, m_credit1UI)
DTPROPERTY(EndSceneManager, m_credit2UI)
DTPROPERTY(EndSceneManager, m_finalCreditUI)
DTPROPERTY(EndSceneManager, m_combatController)
DTPROPERTY(EndSceneManager, m_dialogueText)
ENDPROPERTY()

void EndSceneManager::Start()
{
    NextStep(true); // 첫 시작은 강제로 진입
}

void EndSceneManager::Update(float deltaTime)
{
    if (m_canProceedToNextStep)
    {
        if (InputManager::Instance().GetKeyDown(KeyCode::Space) ||
            InputManager::Instance().GetKeyDown(KeyCode::Enter))
        {
            // 마지막 단계면 타이틀로, 아니면 다음 단계로
            if (m_currentStep == EndStep::EndingCredit)
            {
                ClientSceneManager::Instance().LoadScene("TitleScene");
            }
            else
            {
                NextStep(); // force=false (기본값)
            }
        }
    }


    if (m_currentStep == EndStep::ScreenDarken)
    {
        m_stateTimer += deltaTime;
        float duration = 2.0f; 

        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene && activeScene->GetMainCamera())
        {
            float t = m_stateTimer / duration;
            if (t > 1.0f) t = 1.0f;

            float startSoft = 1.0f;
            float endSoft = -40.0f;
            float currentSoft = startSoft + (endSoft - startSoft) * t;

            activeScene->GetMainCamera()->SetVignetteSoftness(currentSoft);
        }

        if (m_stateTimer >= duration)
        {
            NextStep(true);
        }
    }
}

void EndSceneManager::NextStep(bool force)
{
    if (!force && !m_canProceedToNextStep)
        return;

    int nextIndex = (int)m_currentStep + 1;
    m_currentStep = (EndStep)nextIndex;
    m_stateTimer = 0.0f;

    m_canProceedToNextStep = true;

    switch (m_currentStep)
    {
    case EndStep::Dialogue: {
        if (m_dialogueUI) m_dialogueUI->SetActive(true);
        if (m_dialogueText) m_dialogueText->SetText(L"오호! 정말로 여왕을 이기다니! \n축하해, 이제 네가 이 보드랜드의 여왕이야!");
    }
    break;

    case EndStep::ScreenDarken:
    {
        m_canProceedToNextStep = false;

        if (m_dialogueUI) m_dialogueUI->SetActive(false);

        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene && activeScene->GetMainCamera())
        {
            activeScene->GetMainCamera()->SetUseVignette(true);
            activeScene->GetMainCamera()->SetVignetteSoftness(1.0f); 

            activeScene->GetMainCamera()->SetCircleWidthHeight({ 0.0f, 0.0f });
            activeScene->GetMainCamera()->SetCircleCenter({ 0.5f, 0.5f });
        }
    }
    break;
    case EndStep::DarkCatDialogue: 
    {

        if (m_popupUI) m_popupUI->SetActive(true);
    }
    break;
    case EndStep::Placement: // 3. 배치
    {

        if (m_popupUI) m_popupUI->SetActive(false);
        // 배치는 즉시 처리하고 바로 다음으로
        NextStep(true);
    }
    break;

    case EndStep::ShowPopup: // 4. 팝업
    {
        // 암전 해제
        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene && activeScene->GetMainCamera())
        {
            activeScene->GetMainCamera()->SetUseVignette(false);
        }

        // m_canProceedToNextStep = true 상태 유지 (입력 대기)
    }
    break;

    case EndStep::AliceWalk: // 5. 엘리스 걷기
    {
        m_canProceedToNextStep = false; // 걷는 중 입력 방지

        if (m_combatController) m_combatController->Setup();

        // Update에서 시간 체크 후 자동 넘김
    }
    break;

    case EndStep::Credit_Part1: // 6. 크레딧 1
    {
        if (m_credit1UI) m_credit1UI->SetActive(true);
        // 입력하면 바로 넘어가게 canProceed = true 유지
    }
    break;

    case EndStep::Credit_Part2: // 7. 크레딧 2
    {
        if (m_credit1UI) m_credit1UI->SetActive(false);
        if (m_credit2UI) m_credit2UI->SetActive(true);
    }
    break;

    case EndStep::EndingCredit: // 8. 최종 엔딩
    {
        if (m_credit2UI) m_credit2UI->SetActive(false);
        if (m_finalCreditUI) m_finalCreditUI->SetActive(true);
    }
    break;
    }
}