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
#include "Image.h"

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
    NextStep(true); 
}

void EndSceneManager::Update(float deltaTime)
{
    if (m_canProceedToNextStep)
    {
        if (InputManager::Instance().GetKeyDown(KeyCode::Space) ||
            InputManager::Instance().GetKeyDown(KeyCode::Enter))
        {
            if (m_currentStep == EndStep::EndingCredit)
            {
                //ClientSceneManager::Instance().LoadScene("TitleScene");
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

    if (m_currentStep == EndStep::AliceWalk)
    {
        if (!m_isEndingSequenceStarted)
        {
            if (m_combatController)
            {
                EnemyUnit* enemy0 = m_combatController->GetEnemyUnit0();
                if (enemy0)
                {
                    Vector3 pos = enemy0->GetTransform()->GetPosition();
                    if (pos.z <= 3.0f)
                    {
                        m_isEndingSequenceStarted = true;
                        m_stateTimer = 0.0f;
                    }
                }
            }
        }
        else
        {
            // 3초 대기
            m_stateTimer += deltaTime;
            if (m_stateTimer >= 3.0f)
            {
                NextStep(true); 
            }
        }
    }

    if (m_currentStep == EndStep::PreCreditDarken)
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

    if (IsCreditStep(m_currentStep))
    {
        GameObject* currentUI = nullptr;
        if (m_currentStep == EndStep::Credit_Part1) currentUI = m_credit1UI;
        else if (m_currentStep == EndStep::Credit_Part2) currentUI = m_credit2UI;
        else if (m_currentStep == EndStep::EndingCredit) currentUI = m_finalCreditUI;

        if (m_fadeState == FadeState::FadeIn)
        {
            m_fadeTimer += deltaTime;
            float t = m_fadeTimer / FADE_DURATION;
            if (t >= 1.0f) {
                t = 1.0f;
                m_fadeState = FadeState::Idle;
            }
            SetUIAlpha(currentUI, t);
        }
        else if (m_fadeState == FadeState::Idle)
        {
            SetUIAlpha(currentUI, 1.0f); 

            if (InputManager::Instance().GetKeyDown(KeyCode::Space) ||
                InputManager::Instance().GetKeyDown(KeyCode::Enter))
            {
                m_fadeState = FadeState::FadeOut;
                m_fadeTimer = 0.0f;
            }
        }
        else if (m_fadeState == FadeState::FadeOut)
        {
            m_fadeTimer += deltaTime;
            float t = m_fadeTimer / FADE_DURATION;
            if (t >= 1.0f) {
                t = 1.0f;
                if (m_currentStep == EndStep::EndingCredit)
                {
                    //ClientSceneManager::Instance().LoadScene("TitleScene");
                }
                else
                {
                    NextStep(true);
                }
            }
            SetUIAlpha(currentUI, 1.0f - t);
        }

        return;
    }
}

void EndSceneManager::NextStep(bool force)
{

    if (!force && !m_canProceedToNextStep)
        return;

    int nextIndex = (int)m_currentStep + 1;
    m_currentStep = (EndStep)nextIndex;
    m_stateTimer = 0.0f;

    std::cout << nextIndex << std::endl;

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
    case EndStep::Placement:
    {

        if (m_popupUI) m_popupUI->SetActive(false);
        NextStep(true);
    }
    break;

    case EndStep::ShowPopup:
    {
        // 암전 해제
        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene && activeScene->GetMainCamera())
        {
            activeScene->GetMainCamera()->SetUseVignette(false);
        }

        NextStep(true);

        // m_canProceedToNextStep = true 상태 유지 (입력 대기)
    }
    break;

    case EndStep::AliceWalk: 
    {
        m_canProceedToNextStep = false;


        EnemyUnit* enemy0 = nullptr;
        if (m_combatController) {
            enemy0 = m_combatController->GetEnemyUnit0();
            enemy0->SetPath({ {{2,8},{2,0}} });
            m_combatController->Setup();
        } 

    }
    break;

    case EndStep::PreCreditDarken:
    {
        m_canProceedToNextStep = false; 

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

    case EndStep::Credit_Part1:
    {
        if (m_credit1UI) {
            m_credit1UI->SetActive(true);
            SetUIAlpha(m_credit1UI, 0.0f); // 투명하게 시작
        }
        m_fadeState = FadeState::FadeIn;    // 페이드 인 시작
        m_fadeTimer = 0.0f;
        m_canProceedToNextStep = false;     // Update에서 별도 처리하므로 false
    }
    break;

    case EndStep::Credit_Part2:
    {
        if (m_credit2UI) {
            m_credit2UI->SetActive(true);
            SetUIAlpha(m_credit2UI, 0.0f);
        }
        m_fadeState = FadeState::FadeIn;
        m_fadeTimer = 0.0f;
        m_canProceedToNextStep = false;
    }
    break;

    case EndStep::EndingCredit:
    {
        if (m_finalCreditUI) {
            m_finalCreditUI->SetActive(true);
            SetUIAlpha(m_finalCreditUI, 0.0f);
        }
        m_fadeState = FadeState::FadeIn;
        m_fadeTimer = 0.0f;
        m_canProceedToNextStep = false;
    }
    break;

    }
}

bool EndSceneManager::IsCreditStep(EndStep step)
{
    return step == EndStep::Credit_Part1 ||
        step == EndStep::Credit_Part2 ||
        step == EndStep::EndingCredit;
}

void EndSceneManager::SetUIAlpha(GameObject* target, float alpha)
{
    if (!target) return;

    //Text* textComp = target->GetComponent<Text>();
    //if (textComp)
    //{
    //    Vector4 col = textComp->GetColor();
    //    col.w = alpha; 
    //    textComp->SetColor(col);
    //}

    Image* imgComp = target->GetComponent<Image>();
    if (imgComp)
    {
        Vector4 col = imgComp->GetColor();
        col.w = alpha;
        imgComp->SetColor(col);
    }

}