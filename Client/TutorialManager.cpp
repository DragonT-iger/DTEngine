#include "pch.h"
#include "TutorialManager.h"
#include "GameObject.h"
#include "Text.h"  
#include "UIButton.h" 
#include "InputManager.h"
#include "CombatController.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "VignetteEffect.h"
#include "Prefab.h"
#include "TilemapGenerator.h"
#include "RayCastHitEvent.h"
#include "ArrowObjectPool.h"
#include "GameManager.h"
#include "ClientSceneManager.h"


BEGINPROPERTY(TutorialManager)
DTPROPERTY(TutorialManager, leftChat)
DTPROPERTY(TutorialManager, rightChat)
DTPROPERTY(TutorialManager, leftChatText)
DTPROPERTY(TutorialManager, rightChatText)
DTPROPERTY(TutorialManager, m_queenUI)
DTPROPERTY(TutorialManager, m_catUI)
DTPROPERTY(TutorialManager, m_catUI2)
DTPROPERTY(TutorialManager, m_firstNextButton)
DTPROPERTY(TutorialManager, m_secondNextButton)
DTPROPERTY(TutorialManager, m_catText2)
DTPROPERTY(TutorialManager, m_BattleStart)
DTPROPERTY(TutorialManager, m_victoryUI)
DTPROPERTY(TutorialManager, m_chessSoldier)
DTPROPERTY(TutorialManager, m_combatController)
DTPROPERTY(TutorialManager, m_aliceGameObject)
DTPROPERTY(TutorialManager, m_infoUI);
DTPROPERTY(TutorialManager, m_glowTilePrefab)
DTPROPERTY(TutorialManager, m_redGlowTilePrefab)
DTPROPERTY(TutorialManager, m_tilemapGenerator)
DTPROPERTY(TutorialManager, m_RRSokayButton)
DTPROPERTY(TutorialManager, m_tutorialAdditionalEnemyPrefab)
DTPROPERTY(TutorialManager, m_rayCastHitEvent)
ENDPROPERTY()


void TutorialManager::Start()
{
    NextStep();
}

void TutorialManager::Update(float deltaTime)
{

    //if (InputManager::Instance().GetKeyDown(KeyCode::Num0))
    //{
    //    if (GameManager::Instance() != nullptr)
    //    {
    //        GameManager::Instance()->SetMoney(GameManager::Instance()->GetMoney() + 1);
    //    }
    //    else
    //    {
    //        std::cout << "Error: GameManager instance is null!" << std::endl;
    //    }
    //}



    if (InputManager::Instance().GetKeyDown(KeyCode::Space) || InputManager::Instance().GetKeyDown(KeyCode::Enter))
    {
        if (m_CurrentStep == TutorialStep::Victory) {
            ClientSceneManager::Instance().LoadScene("MainGameScene");
        }
        else {
            NextStep();
        }
    }
    




    if (m_aliceGameObject) {
        AllyUnit* aliceAllyUnit = m_aliceGameObject->GetComponent<AllyUnit>();
        if (!aliceAllyUnit->IsAlive()) {
            if (m_aliceDead == false) {
                m_aliceDead = true;
                m_canProceedToNextStep = true;
                NextStep();
                m_aliceGameObject->GetComponent<Animator>()->SetPlay(true);
            }
        }
    }

    if (m_CurrentStep == TutorialStep::Vignette_FadeIn && m_isVignetteSequence)
    {
        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene)
        {
            m_vignetteDelayTimer += deltaTime;

            if (m_vignetteDelayTimer >= 2.0f)
            {
                Camera* mainCam = activeScene->GetMainCamera();
                if (mainCam)
                {
                    m_currentSoftness -= deltaTime * 15.0f;
                    mainCam->SetVignetteSoftness(m_currentSoftness);

                    if (m_currentSoftness <= -40.0f)
                    {
                        m_currentSoftness = -40.0f;
                        mainCam->SetVignetteSoftness(m_currentSoftness);

                        m_isVignetteSequence = false;
                        m_canProceedToNextStep = true;
                        NextStep();
                    }
                }
            }
        }
    }

    if (m_CurrentStep == TutorialStep::Vignette_FadeOut_CatAppear && m_isVignetteSequence)
    {
        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene)
        {
            Camera* mainCam = activeScene->GetMainCamera();
            if (mainCam)
            {
                m_currentSoftness += deltaTime * 15.0f;
                mainCam->SetVignetteSoftness(m_currentSoftness);


                if (m_currentSoftness > -30.f) {
                    if (m_chessSoldier) m_chessSoldier->SetActive(false);

                    if (m_aliceGameObject) m_aliceGameObject->SetActive(false);
                }

                if (m_currentSoftness >= 1.0f)
                {
                    m_currentSoftness = 1.0f;
                    mainCam->SetVignetteSoftness(m_currentSoftness);

                    m_isVignetteSequence = false;
                    m_canProceedToNextStep = true;

                    if (m_firstNextButton) m_firstNextButton->SetActive(true);
                    if (rightChat) rightChat->SetActive(true);
                    if (m_catUI) m_catUI->SetActive(true);
                    if (rightChatText)
                        rightChatText->SetText(L"이곳에서 나가려면 너만의 규칙을 \n정해야만 하지");
                }
            }
        }
    }

    if (m_CurrentStep == TutorialStep::Cat_Highlight_Tiles)
    {
        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene)
        {
            Camera* mainCam = activeScene->GetMainCamera();
            if (mainCam)
            {
                if (m_circleRadius > 0.15f) {
                    m_circleRadius -= 0.9f * deltaTime;
                }
                else {
                    //m_canProceedToNextStep = true;
                }
                mainCam->SetCircleWidthHeight({ m_circleRadius, m_circleRadius });
            }
        }
    }

    if (m_CurrentStep == TutorialStep::Cat_Explain_Cost && m_isVignetteSequence)
    {
        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene)
        {
            Camera* mainCam = activeScene->GetMainCamera();
            if (mainCam)
            {
                m_vignetteDelayTimer += deltaTime;

                float duration = 0.5f;
                float t = m_vignetteDelayTimer / duration;

                if (t >= 1.0f)
                {
                    t = 1.0f;
                    m_isVignetteSequence = false;
                }

                Vector2 startCenter = Vector2(0.415f, 0.355f);
                Vector2 startWH = Vector2(0.15f, 0.15f);

                Vector2 targetCenter = Vector2(0.770f, 0.350f);
                Vector2 targetWH = Vector2(0.45f, 0.45f);

                Vector2 currentCenter = Vector2::Lerp(startCenter, targetCenter, t);
                Vector2 currentWH = Vector2::Lerp(startWH, targetWH, t);

                mainCam->SetCircleCenter(currentCenter);
                mainCam->SetCircleWidthHeight(currentWH);
            }
        }
    }

    if (m_CurrentStep == TutorialStep::Cat_Explain_Control && m_isVignetteSequence)
    {
        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene)
        {
            Camera* mainCam = activeScene->GetMainCamera();
            if (mainCam)
            {
                m_vignetteDelayTimer += deltaTime;

                float duration = 0.5f;
                float t = m_vignetteDelayTimer / duration;

                if (t >= 1.0f)
                {
                    t = 1.0f;
                    m_isVignetteSequence = false;

                    mainCam->SetCircleCenter({ 0.415f, 0.35f });
                    mainCam->SetCircleWidthHeight({ 0.15f, 0.15f });
                }
                else
                {
                    Vector2 startCenter = Vector2(0.770f, 0.350f);
                    Vector2 startWH = Vector2(0.45f, 0.45f);

                    Vector2 targetCenter = Vector2(0.415f, 0.35f);
                    Vector2 targetWH = Vector2(0.15f, 0.15f);

                    Vector2 currentCenter = Vector2::Lerp(startCenter, targetCenter, t);
                    Vector2 currentWH = Vector2::Lerp(startWH, targetWH, t);

                    mainCam->SetCircleCenter(currentCenter);
                    mainCam->SetCircleWidthHeight(currentWH);
                }
            }
        }
    }

    if (m_CurrentStep == TutorialStep::Cat_Explain_Wait && m_isVignetteSequence)
    {
        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene)
        {
            Camera* mainCam = activeScene->GetMainCamera();
            if (mainCam)
            {
                m_vignetteDelayTimer += deltaTime;

                float duration = 0.5f;
                float t = m_vignetteDelayTimer / duration;

                Vector2 startCenter = Vector2(0.415f, 0.35f);
                Vector2 startWH = Vector2(0.15f, 0.15f);

                Vector2 targetCenter = Vector2(0.770f, 0.350f);
                Vector2 targetWH = Vector2(0.450f, 0.450f);

                if (t >= 1.0f)
                {
                    t = 1.0f;
                    m_isVignetteSequence = false;

                    mainCam->SetCircleCenter(targetCenter);
                    mainCam->SetCircleWidthHeight(targetWH);
                }
                else
                {
                    Vector2 currentCenter = Vector2::Lerp(startCenter, targetCenter, t);
                    Vector2 currentWH = Vector2::Lerp(startWH, targetWH, t);

                    mainCam->SetCircleCenter(currentCenter);
                    mainCam->SetCircleWidthHeight(currentWH);
                }
            }
        }
    }
    if (m_CurrentStep == TutorialStep::Cat_Explain_EnemyIntent)
    {
        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene)
        {
            Camera* mainCam = activeScene->GetMainCamera();
            if (mainCam)
            {
                if (m_circleRadius > 0.15f) {
                    m_circleRadius -= 2.0f * deltaTime;
                    if (m_circleRadius < 0.15f) m_circleRadius = 0.15f;
                }
                mainCam->SetCircleWidthHeight({ m_circleRadius, m_circleRadius });
            }
        }
    }

    if (m_CurrentStep == TutorialStep::Cat_Explain_EnemyPathWait)
    {
        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene)
        {
            Camera* mainCam = activeScene->GetMainCamera();
            if (mainCam)
            {
                if (m_circleRadius < 2.0f) {
                    m_circleRadius += 2.0f * deltaTime; 
                }

                if (m_circleRadius >= 2.0f) {
                    m_circleRadius = 2.0f;
                     mainCam->SetUseCircleMask(false);

                     m_canProceedToNextStep = true;
                }

                mainCam->SetCircleWidthHeight({ m_circleRadius, m_circleRadius });
            }
        }
    }

    if (m_CurrentStep == TutorialStep::Cat_Explain_EnemyPath && m_isVignetteSequence)
    {
        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene)
        {
            Camera* mainCam = activeScene->GetMainCamera();
            if (mainCam)
            {
                m_vignetteDelayTimer += deltaTime;

                float duration = 0.5f; 
                float t = m_vignetteDelayTimer / duration;

                if (t >= 1.0f)
                {
                    t = 1.0f;
                    m_isVignetteSequence = false;

                    mainCam->SetCircleWidthHeight({ 2.0f, 2.0f });
                    mainCam->SetUseCircleMask(false);
                    mainCam->SetCircleCenter({ 0.5f, 0.5f });
                }
                else
                {
                    Vector2 startCenter = Vector2(0.770f, 0.350f);
                    Vector2 startWH = Vector2(0.450f, 0.450f);

                    Vector2 targetCenter = Vector2(0.5f, 0.5f);
                    Vector2 targetWH = Vector2(2.0f, 2.0f);

                    Vector2 currentCenter = Vector2::Lerp(startCenter, targetCenter, t);
                    Vector2 currentWH = Vector2::Lerp(startWH, targetWH, t);

                    mainCam->SetCircleCenter(currentCenter);
                    mainCam->SetCircleWidthHeight(currentWH);
                }
            }
        }
    }
}

void TutorialManager::NextStep(bool force)
{
    if (!force) {
        if (m_canProceedToNextStep == false)
            return;
    }

    int nextStepIndex = (int)m_CurrentStep + 1;
    m_CurrentStep = (TutorialStep)nextStepIndex;

    std::cout << "Step Index: " << nextStepIndex << std::endl;

    switch (m_CurrentStep)
    {
    case TutorialStep::Intro_AliceQuestion: // 0
    {
        if (leftChat) leftChat->SetActive(true);
        if (m_firstNextButton) m_firstNextButton->SetActive(true);

        if (leftChatText)
            leftChatText->SetText(L"여긴 어디지?");
    }
    break;

    case TutorialStep::Intro_WorldExplanation: // 1
    {
        if (leftChat) leftChat->SetActive(false);
        if (rightChat) rightChat->SetActive(true);

        if (rightChatText)
            rightChatText->SetText(L"이곳은 규칙이 지배하는 보드랜드.\n자신만의 규칙을 가지지 못한 자는 이곳에 \n 필요 없느니라!");

        if (m_queenUI) m_queenUI->SetActive(true);
    }
    break;

    case TutorialStep::Setup_CombatUnit: // 2
    {
        m_canProceedToNextStep = false;
        if (m_firstNextButton) m_firstNextButton->SetActive(false);

        if (m_queenUI) m_queenUI->SetActive(false);
        if (rightChat) rightChat->SetActive(false);

        if (m_chessSoldier) m_chessSoldier->SetActive(true);


        if (m_combatController) m_combatController->GetEnemyUnit0()->SetPath({ {{ 1.f,5.f },{ 5.f,5.f }} });
        if (m_combatController) {
            UnitStats stat = m_combatController->GetEnemyUnit0()->GetStats();
            stat.atk = 60;

            m_combatController->GetEnemyUnit0()->SetStats(stat);
        }
        if (m_combatController) m_combatController->Setup();
    }
    break;

    case TutorialStep::Vignette_FadeIn: // 3
    {
        m_canProceedToNextStep = false;

        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene)
        {
            Camera* mainCam = activeScene->GetMainCamera();
            if (mainCam)
            {
                mainCam->SetUseVignette(true);
                m_currentSoftness = 1.0f;
                m_isVignetteSequence = true;
                m_vignetteDelayTimer = 0.0f;
            }
        }
    }
    break;

    case TutorialStep::Vignette_FadeOut_CatAppear: // 4
    {
        if (m_combatController) m_combatController->SetAllyUnit0(nullptr);
        if (m_combatController) m_combatController->SetEnemyUnit0(nullptr);
        m_canProceedToNextStep = false;
        m_isVignetteSequence = true;
    }
    break;

    case TutorialStep::Alice_AskRule: // 5
    {
        if (rightChat) rightChat->SetActive(false);
        if (leftChat) leftChat->SetActive(true);
        if (leftChatText) leftChatText->SetText(L"규칙을 어떻게 정해야 하지?");
    }
    break;

    case TutorialStep::Cat_OfferHelp: // 6
    {
        if (rightChat) rightChat->SetActive(true);
        if (leftChat) leftChat->SetActive(false);
        if (rightChatText) rightChatText->SetText(L"심심하던 참이니 \n내가 한번 도와줘 볼까?");
    }
    break;

    case TutorialStep::Cat_HideChat: // 7
    {
        if (rightChat) rightChat->SetActive(false);
        NextStep(true);
    }
    break;

    case TutorialStep::Cat_Explain_NeedSoldier: // 8
    {
        if (m_catUI) m_catUI->SetActive(false);
        if (m_firstNextButton) m_firstNextButton->SetActive(false);

        if (m_secondNextButton) m_secondNextButton->SetActive(true);
        if (m_catUI2) m_catUI2->SetActive(true);
        if (m_catText2)
            m_catText2->SetText(L"네 자신을 지키려면 \n체스 병사의 도움이 필요해!");

        if (m_infoUI) m_infoUI->SetActive(false);
    }
    break;

    case TutorialStep::Cat_Highlight_Tiles: // 9
    {
        if (m_catText2)
            m_catText2->SetText(L"빛나는 타일을 눌러봐");

        m_canProceedToNextStep = false;

          if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(4, 5, m_glowTilePrefab);
        //if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(3, 6, m_glowTilePrefab);

        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene) 
        {
            Camera* mainCam = activeScene->GetMainCamera();
            if (mainCam)
            {
                mainCam->SetUseVignette(false);
                mainCam->SetUseCircleMask(true);

                mainCam->SetCircleCenter({ 0.415, 0.355 });
                m_circleRadius = 1.0f;
                mainCam->SetCircleWidthHeight({ m_circleRadius, m_circleRadius });

                m_currentSoftness = 1.0f;
                m_isVignetteSequence = true;
                m_vignetteDelayTimer = 0.0f;
            }
        }
        m_rayActive = true;
    }
    break;

    case TutorialStep::Cat_Explain_Cost: // 10
    {
        if (m_catText2)
            m_catText2->SetText(L"해당 위치에 코스트를 지불하고 \n체스 병사를 소환할 수 있어");
        m_canProceedToNextStep = false;
        m_isVignetteSequence = true;
        m_vignetteDelayTimer = 0.0f;
       
    }
    break;

    case TutorialStep::Cat_Order_Summon: // 11
    {
        m_combatController->GetAllyUnit0()->_GetOwner()->GetTransform()->SetRotationEuler({ 0,180,0 });
        NextStep(true);
    }
    break;

    case TutorialStep::Cat_Explain_Control: // 12
    {
        if (m_catText2) {
            m_catText2->SetText(L"병사를 선택해서 이동과 전투\n규칙을 정해줄 수 있어");
        }
        m_isVignetteSequence = true;
        m_vignetteDelayTimer = 0.0f;
    }
    break;

    case TutorialStep::Cat_Explain_Wait: // 13
    {
        if (m_catText2) {
            m_catText2->SetText(L"대기 규칙을 적용하면\n병사가 방어타일로 이동해");
        }
        m_isVignetteSequence = true;
        m_vignetteDelayTimer = 0.0f;
        m_RRSokayButton->SetActive(false);
    }
    break;

    case TutorialStep::Cat_Strategy_Closest: // 14
    {
        if (m_catText2) {
            m_catText2->SetText(L"적은 하나뿐이니,\n이번엔 체력이 낮은 놈을 노려 보지");
        }
    }
    break;

    case TutorialStep::Cat_Order_Confirm: // 15
    {
        if (m_catText2) {
            m_catText2->SetText(L"결정을 마쳤다면,\n확인을 눌러.");
        }

        m_rayActive = false;
        m_RRSokayButton->SetActive(true);
    }
    break;

    case TutorialStep::Cat_Explain_EnemyPath: // 16
    {
        m_canProceedToNextStep = true;
        if (m_catText2) {
            m_catText2->SetText(L"참, 적은 지정된 경로를\n따라서 움직여.");
        }
        m_tutorialAdditionalEnemy = m_tutorialAdditionalEnemyPrefab->Instantiate();


        m_tutorialAdditionalEnemy->GetTransform()->SetPosition({ 0 , 1 ,12 });
        m_tutorialAdditionalEnemy->GetTransform()->SetRotationEuler({ 0, -90, 0 });
        m_tutorialAdditionalEnemy->GetComponent<EnemyUnit>()->SetPos({ 0 ,6 });
        m_tutorialAdditionalEnemy->GetComponent<EnemyUnit>()->SetBoss(true);

        m_combatController->SetEnemyUnit0(m_tutorialAdditionalEnemy->GetComponent<EnemyUnit>());

        if (m_combatController) m_combatController->GetEnemyUnit0()->SetPath({ {{ 0.f,6.f },{ 4.f,6.f }} });
        m_isVignetteSequence = true;
        m_vignetteDelayTimer = 0.0f;
    }
    break;

    case TutorialStep::Cat_Explain_EnemyIntent: // 17
    {
        if (m_catText2) {
            m_catText2->SetText(L"적을 누르면 그 녀석이 어디로 움직일지,\n확인할 수 있지.");
        }

        m_canProceedToNextStep = false;

        m_rayActive = true;

        Scene* activeScene = SceneManager::Instance().GetActiveScene();
        if (activeScene)
        {
            Camera* mainCam = activeScene->GetMainCamera();
            if (mainCam)
            {
                mainCam->SetUseCircleMask(true);
                mainCam->SetCircleCenter({ 0.755f, 0.270f });

                m_circleRadius = 1.5f;
                mainCam->SetCircleWidthHeight({ m_circleRadius, m_circleRadius });
            }
        }
    }
    break;

    case TutorialStep::Cat_Explain_EnemyPathWait:
    {
        m_rayActive = false;
    }
    break;

    case TutorialStep::Cat_Strategy_DefenseTile: 
    {
        if (m_catText2) {
            m_catText2->SetText(L"잘됐네 , 딱 보니까 방어 타일 위에 있으면 \n저 녀석을 막기 좋겠어.");
        }
        m_rayCastHitEvent->GetArrowPool()->_GetOwner()->SetActive(false);

        if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(4, 7, m_glowTilePrefab);

        m_combatController->GetBattleGrid()->SetDefenseTile({ 3,6 });

    }
    break;

    case TutorialStep::Cat_Order_StartBattle:
    {
        if (m_catText2) {
            m_catText2->SetText(L"자, 모든 규칙을 정했다면 \n싸움을 붙여 보자고!");
        }

        m_canProceedToNextStep = true;
        if (m_BattleStart) m_BattleStart->SetActive(true);
    }
    break;

    case TutorialStep::Battle:
    {

        m_combatController->Setup();
        UnitStats stat = m_combatController->GetAllyUnit0()->GetStats();
        stat.atk = 60;

        m_combatController->GetAllyUnit0()->SetStats(stat);


        //if (m_catText2) {
        //    m_catText2->SetText(L"아, 참! 깜빡할 뻔했는데,\n이 토끼 녀석들은\n공격할 수 있는 범위가 정해져 있어.!");
        //}
        if (m_BattleStart) m_BattleStart->SetActive(false);
        
        m_canProceedToNextStep = false;
    }
    break;

    case TutorialStep::Cat_Explain_EnemyRange:
    {
        if (m_catText2) {
            m_catText2->SetText(L"아, 참!, 이 토끼 녀석들은\n공격할 수 있는 범위가 정해져 있어.!");
        }
        m_canProceedToNextStep = true;

        if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(3, 6, m_glowTilePrefab);
        if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(3, 7, m_redGlowTilePrefab);
        if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(3, 8, m_glowTilePrefab);
        if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(4, 6, m_redGlowTilePrefab);
        if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(4, 7, m_glowTilePrefab);     // 이건 수정하긴해야됨
        if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(4, 8, m_redGlowTilePrefab);
        if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(5, 6, m_glowTilePrefab);
        if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(5, 7, m_redGlowTilePrefab);
        if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTile(5, 8, m_glowTilePrefab);
    }
    break;

    case TutorialStep::Cat_Explain_EnemyRange_End: 
    {
        m_combatController->GetEnemyUnit0()->_GetOwner()->SetActive(false);
        if (m_catText2) {
            m_catText2->SetText(L"뭐, 알고 있으면 좋을 거야.\n너를 위해서 말이야.");
        }
        if (m_BattleStart) m_BattleStart->SetActive(false);
    }
    break;


    case TutorialStep::Victory:
    {
        if (m_catUI2) m_catUI2->SetActive(false);
        if (m_victoryUI) m_victoryUI->SetActive(true);
    }
    break;

    default:
        std::cout << "튜토리얼 끝" << std::endl;
        break;
    }
}