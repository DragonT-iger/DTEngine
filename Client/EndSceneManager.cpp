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
#include "BattleGrid.h"
#include "TilemapGenerator.h"
#include "TilemapData.h"
#include "AliceUnit.h"
#include "Transform.h"
#include "SoundManager.h"

BEGINPROPERTY(EndSceneManager)
DTPROPERTY(EndSceneManager, m_aliceDialogueUI)
DTPROPERTY(EndSceneManager, m_aliceDialogueText)
DTPROPERTY(EndSceneManager, m_chesherDialogueUI)
DTPROPERTY(EndSceneManager, m_chesherDialogueText)
DTPROPERTY(EndSceneManager, m_popupUI)
DTPROPERTY(EndSceneManager, m_credit1UI)
DTPROPERTY(EndSceneManager, m_credit2UI)
DTPROPERTY(EndSceneManager, m_finalCreditUI)
DTPROPERTY(EndSceneManager, m_combatController)
DTPROPERTY(EndSceneManager, m_additionalFall1)
DTPROPERTY(EndSceneManager, m_chatNext)
//DTPROPERTY(EndSceneManager, m_additionalFall2)
ENDPROPERTY()

void EndSceneManager::Start()
{
    SoundManager::Instance().StopBGM(true); // 일단 임시로 넣어둠.. 
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
        if (m_combatController)
        {
            EnemyUnit* enemy0 = m_combatController->GetEnemyUnit0();
            if (enemy0)
            {
                Vector3 pos = enemy0->GetTransform()->GetPosition();

                if (pos.z <= 2.0f)
                {
                    m_combatController->_GetOwner()->SetActive(false);

                }

                if (!m_isEndingSequenceStarted)
                {
                    if (pos.z <= 3.0f)
                    {
                        m_isEndingSequenceStarted = true;
                        m_stateTimer = 0.0f;
                    }
                }
            }
        }

        if (m_isEndingSequenceStarted)
        {
            m_stateTimer += deltaTime;
            if (m_stateTimer >= 3.0f)
            {
                NextStep(true);
            }
        }
    }

    if (m_currentStep == EndStep::DarkCatDialogue)
    {
        if (m_currentStep == EndStep::DarkCatDialogue)
        {
            if (m_isTyping)
            {
                // 스페이스바/엔터로 타이핑 스킵
                if (InputManager::Instance().GetKeyDown(KeyCode::Space) ||
                    InputManager::Instance().GetKeyDown(KeyCode::Enter))
                {
                    if (m_popupUI)
                    {
                        Text* textComp = m_popupUI->GetComponent<Text>();
                        if (textComp) textComp->SetText(m_targetPopupText);
                    }
                    m_isTyping = false;
                    m_stateTimer = 0.0f; 
                }
                else
                {
                    m_typingTimer += deltaTime;
                    if (m_typingTimer >= TYPING_SPEED)
                    {
                        m_typingTimer = 0.0f;
                        m_typingIndex++;

                        if (m_popupUI)
                        {
                            Text* textComp = m_popupUI->GetComponent<Text>();
                            if (textComp)
                            {
                                if (m_typingIndex <= m_targetPopupText.length())
                                {
                                    textComp->SetText(m_targetPopupText.substr(0, m_typingIndex));
                                }
                                else
                                {
                                    m_isTyping = false;
                                    m_stateTimer = 0.0f; 
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                m_stateTimer += deltaTime;
                float waitDuration = 2.0f; 

                if (m_stateTimer >= waitDuration)
                {
                    NextStep(true);
                }
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

    if (m_currentStep == EndStep::TileShake)
    {
        m_stateTimer += deltaTime;
        float duration = 2.0f;

        for (auto& tile : m_targetTiles)
        {
            if (!tile.obj) continue;

            float offsetX = ((rand() % 100) / 100.0f - 0.5f) * m_shakeIntensity;
            float offsetZ = ((rand() % 100) / 100.0f - 0.5f) * m_shakeIntensity;
            float offsetY = ((rand() % 100) / 100.0f - 0.5f) * m_shakeIntensity * 0.5f;

            tile.obj->GetTransform()->SetPosition(tile.originalPos + Vector3(offsetX, offsetY, offsetZ));
        }

        if (m_stateTimer >= duration)
        {
            NextStep(true);
        }
    }

    if (m_currentStep == EndStep::TileFall)
    {
        m_stateTimer += deltaTime;
        float duration = 3.0f; 

        m_fallSpeed += 9.8f * deltaTime * 2.0f;



        Vector3 alicePos = Vector3(0, 0, 0);
        GameObject* aliceObj = nullptr;

        if (m_combatController)
        {
            auto* unit = m_combatController->GetEnemyUnit0();
            if (unit)
            {
                aliceObj = unit->_GetOwner();
                alicePos = aliceObj->GetTransform()->GetPosition();
            }
        }

        for (auto& tile : m_targetTiles)
        {
            if (!tile.obj) continue;

            Vector3 currentPos = tile.obj->GetTransform()->GetPosition();

            float diffX = std::abs(currentPos.x - alicePos.x);
            float diffZ = std::abs(currentPos.z - alicePos.z);

            float range = 4.1f;          
            float sameLineMargin = 0.5f; 
            float diagRange = 2.1f;      

            bool isCross = (diffX < sameLineMargin && diffZ < range) ||
                (diffZ < sameLineMargin && diffX < range);

            bool isDiagonal = (diffX < diagRange && diffZ < diagRange);

            if (isCross || isDiagonal)
            {
                currentPos.y -= m_fallSpeed * deltaTime;
                tile.obj->GetTransform()->SetPosition(currentPos);
            }
        }

        if (aliceObj)
        {
            Vector3 currentAlicePos = aliceObj->GetTransform()->GetPosition();
            currentAlicePos.y -= m_fallSpeed * deltaTime;
            aliceObj->GetTransform()->SetPosition(currentAlicePos);
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
                m_fadeTimer = 0.0f; 
            }
            SetUIAlpha(currentUI, t);
        }
        else if (m_fadeState == FadeState::Idle)
        {
            m_fadeTimer += deltaTime; 
            SetUIAlpha(currentUI, 1.0f);

            float autoPassTime = 3.0f; 

            if (m_fadeTimer >= autoPassTime ||
                InputManager::Instance().GetKeyDown(KeyCode::Space) ||
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
                NextStep(true);
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

    //std::cout << nextIndex << std::endl;

    m_canProceedToNextStep = true;

    switch (m_currentStep)
    {
    case EndStep::Dialogue_01:
        // 체셔 캣
        if (m_aliceDialogueUI) m_aliceDialogueUI->SetActive(false);
        if (m_chesherDialogueUI) m_chesherDialogueUI->SetActive(true);
        if (m_chesherDialogueText) m_chesherDialogueText->SetText(L"오호! 정말로 여왕을 이기다니.\n축하해, 앨리스. 이제 네가 이 보드랜드의 여왕이야!");
        break;

    case EndStep::Dialogue_02:
        // 앨리스
        if (m_chesherDialogueUI) m_chesherDialogueUI->SetActive(false);
        if (m_aliceDialogueUI) m_aliceDialogueUI->SetActive(true);
        if (m_aliceDialogueText) m_aliceDialogueText->SetText(L"뭐라고? 여왕 같은 건 되고 싶지 않아!\n난 그냥… 그냥 집으로 돌아가고 싶어!");
        break;

    case EndStep::Dialogue_03:
        // 체셔 캣
        if (m_aliceDialogueUI) m_aliceDialogueUI->SetActive(false);
        if (m_chesherDialogueUI) m_chesherDialogueUI->SetActive(true);
        if (m_chesherDialogueText) m_chesherDialogueText->SetText(L"그래? 그렇다면 규칙을 만들면 되잖아?");
        break;

    case EndStep::Dialogue_04:
        // 앨리스
        if (m_chesherDialogueUI) m_chesherDialogueUI->SetActive(false);
        if (m_aliceDialogueUI) m_aliceDialogueUI->SetActive(true);
        if (m_aliceDialogueText) m_aliceDialogueText->SetText(L"규칙이라고?");
        break;

    case EndStep::Dialogue_05:
        // 체셔 캣
        if (m_aliceDialogueUI) m_aliceDialogueUI->SetActive(false);
        if (m_chesherDialogueUI) m_chesherDialogueUI->SetActive(true);
        if (m_chesherDialogueText) m_chesherDialogueText->SetText(L"그래, 여왕 폐하.내가 말했잖아? \n보드랜드의 규칙을 만드는 건 여왕이라고.");
        break;

    case EndStep::Dialogue_06:
        // 체셔 캣 (연속 대화)
        if (m_chesherDialogueText) m_chesherDialogueText->SetText(L"지금 보드랜드의 여왕은 너지,\n그러니 규칙을 만드는 것도 너야.");
        break;

    case EndStep::Dialogue_07:
        // 앨리스
        if (m_chesherDialogueUI) m_chesherDialogueUI->SetActive(false);
        if (m_aliceDialogueUI) m_aliceDialogueUI->SetActive(true);
        if (m_aliceDialogueText) m_aliceDialogueText->SetText(L"어떻게 해야 하는데?\n토끼 병사들을 움직이듯이 하면 돼?");
        break;

    case EndStep::Dialogue_08:
        // 체셔 캣
        if (m_aliceDialogueUI) m_aliceDialogueUI->SetActive(false);
        if (m_chesherDialogueUI) m_chesherDialogueUI->SetActive(true);
        if (m_chesherDialogueText) m_chesherDialogueText->SetText(L"자, 눈을 감아 봐.\n내가 안내해 주지.");
        break;

    case EndStep::Dialogue_09:
        // 체셔 캣
        if (m_chesherDialogueText) m_chesherDialogueText->SetText(L"돌아가서도 잊지 말라고.\n이 세상을 지배하는 건…");
        break;

    case EndStep::ScreenDarken:
    {
        m_canProceedToNextStep = false;

        if (m_aliceDialogueUI) m_aliceDialogueUI->SetActive(false);
        if (m_chesherDialogueUI) m_chesherDialogueUI->SetActive(false);
        if (m_chatNext) m_chatNext->SetActive(false);

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
        if (m_popupUI)
        {
            m_popupUI->SetActive(true);

            Text* textComp = m_popupUI->GetComponent<Text>();
            if (textComp)
            {
                m_targetPopupText = textComp->GetText(); 
                textComp->SetText(L""); 

                m_isTyping = true;
                m_typingIndex = 0;
                m_typingTimer = 0.0f;

                m_canProceedToNextStep = false;
            }
        }
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
            enemy0->SetPath({ {{3,8},{3,1}} });
            m_combatController->Setup();
        } 

    }
    break;


    case EndStep::TileShake:
    {
        m_canProceedToNextStep = false; // 자동 진행
        PrepareTileEffect(); // 타일 정보 수집 및 초기화
    }
    break;

    case EndStep::TileFall:
    {
        m_canProceedToNextStep = false; // 자동 진행
        m_fallSpeed = 0.0f; // 속도 초기화
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

    case EndStep::ReturnToMain:
    {
        ClientSceneManager::Instance().LoadScene("TitleScene");
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



void EndSceneManager::PrepareTileEffect()
{
    m_targetTiles.clear();

    if (!m_combatController) return;

    BattleGrid* grid = m_combatController->GetBattleGrid();
    if (!grid) return;

    TilemapGenerator* generator = grid->GetTilemapGenerator();
    if (!generator) return;

    GameObject* aliceObj = generator->GetSpawnedAlice();
    Vector2 aliceGridPos = { -1, -1 };

    if (aliceObj)
    {
        AliceUnit* aliceUnit = aliceObj->GetComponent<AliceUnit>();
        if (aliceUnit)
        {
            aliceGridPos = aliceUnit->GetPos();
        }
    }

    auto& tiles = generator->GetSpawnedTiles();
    TilemapData* mapData = generator->GetMapData();
    if (!mapData) return;

    int width = mapData->GetExpandedWidth();

    for (int i = 0; i < tiles.size(); ++i)
    {
        GameObject* tile = tiles[i];
        if (!tile) continue;

        int x = i % width;
        int y = i / width;

        m_targetTiles.push_back({ tile, tile->GetTransform()->GetPosition() });
    }
}