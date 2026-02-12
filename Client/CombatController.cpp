#include "CombatController.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Scene.h"
#include "TutorialManager.h"
#include "GameManager.h"
#include "ClientSceneManager.h"
#include "EffectManager.h"

#include "BgFadeController.h"
#include "UIButton.h"
BEGINPROPERTY(CombatController)
DTPROPERTY_SETTER(CombatController, battleGrid, SetBattleGrid)
DTPROPERTY_SETTER(CombatController, m_aliceUnit, SetAliceUnit)
DTPROPERTY_SETTER(CombatController, m_redQueenUnit, SetRedQueenUnit)
DTPROPERTY_SETTER(CombatController, allyUnit0, SetAllyUnit0)
DTPROPERTY_SETTER(CombatController, allyUnit1, SetAllyUnit1)
DTPROPERTY_SETTER(CombatController, allyUnit2, SetAllyUnit2)
DTPROPERTY_SETTER(CombatController, enemyUnit0, SetEnemyUnit0)
DTPROPERTY_SETTER(CombatController, enemyUnit1, SetEnemyUnit1)
DTPROPERTY_SETTER(CombatController, enemyUnit2, SetEnemyUnit2)

DTPROPERTY(CombatController, m_victoryBG)
DTPROPERTY(CombatController, m_loseBG)
DTPROPERTY(CombatController, m_overBG)
DTPROPERTY(CombatController, m_healskillButton)
DTPROPERTY(CombatController, m_attackskillButton)
ENDPROPERTY()


CombatController::~CombatController()
{
    battleGrid = nullptr;

    m_aliceUnit = nullptr;
    m_redQueenUnit = nullptr;

    allyUnit0 = nullptr;
    allyUnit1 = nullptr;
    allyUnit2 = nullptr;

    enemyUnit0 = nullptr;
    enemyUnit1 = nullptr;
    enemyUnit2 = nullptr;

    m_allyUnits.clear();
    m_enemyUnits.clear();
    m_battleUnits.clear();
}

void CombatController::Start()
{
    //Setup();
}

void CombatController::Update(float dTime)
{
    Process(dTime);
}

void CombatController::Setup()
{
    // 배틀맵을 통해 타일맵 가져와서 적 정보 가져와서 등록해주기.

    if (SceneManager::Instance().GetActiveScene()->GetName() != "TutorialScene" && SceneManager::Instance().GetActiveScene()->GetName() != "EndingScene")
    {
        int currentLevel = GameManager::Instance()->GetStageLevel();

        if (currentLevel == 1 || currentLevel == 2 || currentLevel == 3 || currentLevel == 4)
        {
            SoundManager::Instance().PlayBGM("BGM/BGM_Battle_ver.2", true);
        }
        else if (currentLevel == 5 || currentLevel == 6)
        {
            SoundManager::Instance().PlayBGM("BGM/Boss_Battle", true);
        }
    }
    
    m_stageEnd = false;                     
    m_currPhase = Phase::Ready;             
    m_stageResult = StageResult::InProgress;
    m_phaseEntered = false;

    // 전투 시작 시 결과 ui 초기화.
    //if (GameManager::Instance())
    //{
    //    GameManager::Instance()->ResetBattleResultUI();
    //}

    m_allyUnits.clear();
    m_enemyUnits.clear();
    m_battleUnits.clear();
    
    if (allyUnit0 && std::find(m_allyUnits.begin(), m_allyUnits.end(), allyUnit0) == m_allyUnits.end()) // 중복 체크 추가.
        m_allyUnits.push_back(allyUnit0);
    if (allyUnit1 && std::find(m_allyUnits.begin(), m_allyUnits.end(), allyUnit1) == m_allyUnits.end())
        m_allyUnits.push_back(allyUnit1);
    if (allyUnit2 && std::find(m_allyUnits.begin(), m_allyUnits.end(), allyUnit2) == m_allyUnits.end())
        m_allyUnits.push_back(allyUnit2);

    if (SceneManager::Instance().GetActiveScene()->GetName() == "TutorialScene" || SceneManager::Instance().GetActiveScene()->GetName() == "EndingScene") {

        if (enemyUnit0 && std::find(m_enemyUnits.begin(), m_enemyUnits.end(), enemyUnit0) == m_enemyUnits.end())
            m_enemyUnits.push_back(enemyUnit0);
        if (enemyUnit1 && std::find(m_enemyUnits.begin(), m_enemyUnits.end(), enemyUnit1) == m_enemyUnits.end())
            m_enemyUnits.push_back(enemyUnit1);
        if (enemyUnit2 && std::find(m_enemyUnits.begin(), m_enemyUnits.end(), enemyUnit2) == m_enemyUnits.end())
            m_enemyUnits.push_back(enemyUnit2);
    //if (enemyUnit0 && std::find(m_enemyUnits.begin(), m_enemyUnits.end(), enemyUnit0) == m_enemyUnits.end())
    //    m_enemyUnits.push_back(enemyUnit0);
    //if (enemyUnit1 && std::find(m_enemyUnits.begin(), m_enemyUnits.end(), enemyUnit1) == m_enemyUnits.end())
    //    m_enemyUnits.push_back(enemyUnit1);
    //if (enemyUnit2 && std::find(m_enemyUnits.begin(), m_enemyUnits.end(), enemyUnit2) == m_enemyUnits.end()) 
    //    m_enemyUnits.push_back(enemyUnit2);




    }
    else {
        std::vector<GameObject*> enemyObjects = battleGrid->GetEnemyObjects();

       for (int i = 0; i < enemyObjects.size(); ++i)
       {
           if (enemyObjects[i])
           {
               EnemyUnit* enemy = enemyObjects[i]->GetComponent<EnemyUnit>();
               if (enemy) 
               { 
                   AddEnemyUnit(enemy);
                   m_enemyUnits.push_back(enemy); 
               }
           }
       }

       //for (EnemyUnit* enemy : m_enemyUnits)
       //{
       //    if (enemy) enemy->SetPath0();
       //}
    }

    GameObject* aliceObject = battleGrid->GetAliceObjects();
    if (aliceObject)
    {
        AliceUnit* alice = aliceObject->GetComponent<AliceUnit>();
        if (alice) 
        { 
            m_aliceUnit = alice; 

            Vector3 pos = aliceObject->GetTransform()->GetPosition();
            Vector2 p0;
            p0.x = std::round(pos.x / 2.0f);
            p0.y = std::round(pos.z / 2.0f);

            m_aliceUnit->SetPos(p0);
        }

    }

    GameObject* queenObject = battleGrid->GetRedQueenObjects();
    if (queenObject)
    {
        RedQueenUnit* queen = queenObject->GetComponent<RedQueenUnit>();
        if (queen) 
        {
            m_redQueenUnit = queen;

            Vector3 pos = queenObject->GetTransform()->GetPosition();
            Vector2 p0;
            p0.x = std::round(pos.x / 2.0f);
            p0.y = std::round(pos.z / 2.0f);

            m_redQueenUnit->SetPos(p0);
        }
    }

    m_isStageStart = true;
}

bool CombatController::ReadyPhase()
{
    m_battleUnits.clear();

    battleGrid->ClearDynamicGrid();
    battleGrid->SyncUnitsPos(m_allyUnits, m_enemyUnits, m_aliceUnit, m_redQueenUnit);

    if (m_aliceUnit)
    {
        m_aliceUnit->SetMovePos(m_aliceUnit->GetPos()); // 앨리스는 예외로 이동 안함.
        battleGrid->ReserveMove(m_aliceUnit->GetPos());
    }

    if (m_redQueenUnit)
    {
        m_redQueenUnit->SetMovePos(m_redQueenUnit->GetPos()); // 붉은 여왕도 예외로 이동 안함.
        battleGrid->ReserveMove(m_redQueenUnit->GetPos());
    }

    for (AllyUnit* ally : m_allyUnits)
    {
        if (!ally || !ally->IsAlive()) continue;

        std::vector<Unit*> targets;
        GetAttackableTargets(ally, targets);

        if (!targets.empty())
        {
            m_battleUnits.push_back(ally);

            Unit* target = SelectAttackTarget(ally, targets, ally->GetBattleRule());
            ally->SetAttackTarget(target);
        }
        else
        {
            ally->SetAttackTarget(nullptr);
        }
    }

    for (EnemyUnit* enemy : m_enemyUnits)
    {
        if (!enemy || !enemy->IsAlive()) continue;

        std::vector<Unit*> targets;
        GetAttackableTargets(enemy, targets);

        if (!targets.empty())
        {
            m_battleUnits.push_back(enemy);

            Unit* target = SelectAttackTarget(enemy, targets, BattleRule::Nearest);
            enemy->SetAttackTarget(target);
        }
        else
        {
            enemy->SetAttackTarget(nullptr);
        }
    }

    return true;
}

bool CombatController::ReadyToMovePhase()
{
    for (AllyUnit* ally : m_allyUnits)
    {
        if (!ally || !ally->IsAlive()) continue;
        if (IsBattleUnit(ally)) continue; // 전투목록에 있으면 이동 안함.

        Vector2 moveTarget = DecideMoveTarget_Ally(ally);
        ally->SetMoveTargetPos(moveTarget);

        Vector2 movePos = DecideMovePos_Ally(ally, moveTarget);

        // 최종 확정 먼저
        //if (movePos == GRIDPOS_INVALID) movePos = ally->GetPos();
        if (movePos == GRIDPOS_INVALID 
            || battleGrid->IsBlocked(movePos, ally, nullptr, ally->GetMoveRule())) movePos = ally->GetPos();
        ally->SetMovePos(movePos);

        //auto p = ally->GetPos();
        //auto mp = ally->GetMovePos();

        // 액션 확정
        if (ally->GetMovePos() != ally->GetPos()) ally->SetAction(TurnAction::Move);
        else ally->SetAction(TurnAction::Wait);

        // 예약은 마지막에, 확정값으로만
        battleGrid->ReserveMove(movePos);
    }

    for (EnemyUnit* enemy : m_enemyUnits)
    {
        if (!enemy || !enemy->IsAlive()) continue;
        if (IsBattleUnit(enemy)) continue; // 전투목록에 있으면 이동 안함.

        Vector2 movePos = DecideMovePos_Enemy(enemy);
        enemy->SetMovePos(movePos); // 이동지점 설정.

        if (enemy->GetMovePos() != enemy->GetPos()) enemy->SetAction(TurnAction::Move);
        else enemy->SetAction(TurnAction::Wait);

        battleGrid->ReserveMove(movePos);
    }

    return true;
}

bool CombatController::ReadyForBattlePhase()
{
    for (Unit* me : m_battleUnits)
    {
        if (!me || !me->IsAlive()) continue;
        me->SetMovePos(me->GetPos());
        battleGrid->ReserveMove(me->GetPos());
    }

    for (Unit* me : m_battleUnits)
    {
        Vector2 attackPos = DecideAttackPos(me);

        if (attackPos != GRIDPOS_INVALID) me->SetAttackPos(attackPos);

        TurnAction ta = me->GetAction();
        if (ta != TurnAction::Miss && ta != TurnAction::BreakWall) me->SetAction(TurnAction::Attack);
    }

    return true;
}

bool CombatController::MoveAndBattlePhase(float dTime)
{
    //std::cout << "move" << std::endl;
    if (!m_phaseEntered) // 이번 단계 처음 들어옴.
    {
        m_phaseEntered = true;

        //PrintFrame();

        for (AllyUnit* ally : m_allyUnits)
        {
            if (!ally || !ally->IsAlive()) continue;

            if (ally->GetPos() != ally->GetMovePos() && battleGrid->IsTrapTile(ally->GetMovePos()))
            {
                ally->SetOnTrapTile(true);
            }

            ResolveTurnAction(ally);
            //if (battleGrid->IsTrapTile(ally->GetPos()))
            //{
            //    ally->TakeDamage(m_trapDamage);
            //}

            ally->SetActionDone(false);
            ally->StartAction();   // 애니메이션 시작
        }

        for (EnemyUnit* enemy : m_enemyUnits)
        {
            if (!enemy || !enemy->IsAlive()) continue;

            if (enemy->GetPos() != enemy->GetMovePos() && battleGrid->IsTrapTile(enemy->GetMovePos()))
            {
                enemy->SetOnTrapTile(true);
            }

            ResolveTurnAction(enemy);
            //if (battleGrid->IsTrapTile(enemy->GetPos()))
            //{
            //    enemy->TakeDamage(m_trapDamage);
            //}

            enemy->SetActionDone(false);
            enemy->StartAction();
        }

        return false;
    }

    for (AllyUnit* ally : m_allyUnits)
    {
        if (ally && ally->IsAlive()) ally->UpdateAction(dTime);
    }

    for (EnemyUnit* enemy : m_enemyUnits)
    {
        if (enemy && enemy->IsAlive()) enemy->UpdateAction(dTime);
    }

    // 실제 이동 및 애니메이션 완료 체크
    for (AllyUnit* ally : m_allyUnits)
    {
        if (!ally || !ally->IsAlive()) continue;

        if (!ally->IsActionDone()) return false;
        if (ally->GetAction() == TurnAction::Wait) continue;
        if (ally->IsAnimStart() && !ally->IsAnimDone()) return false;

        //ApplyActionResult(ally);
    }

    for (EnemyUnit* enemy : m_enemyUnits)
    {
        if (!enemy || !enemy->IsAlive()) continue;

        if (!enemy->IsActionDone()) return false;
        if (enemy->GetAction() == TurnAction::Wait) continue;
        if (enemy->IsAnimStart() && !enemy->IsAnimDone()) return false;

        //ApplyActionResult(enemy);
    }

    // 함정타일 밟았을 때, 데미지 처리
    for (AllyUnit* ally : m_allyUnits)
    {
        if (!ally || !ally->IsAlive()) continue;

        if (ally->IsOnTrapTile())
        {
            ally->TakeDamage(m_trapDamage);
        }
    }

    for (EnemyUnit* enemy : m_enemyUnits)
    {
        if (!enemy || !enemy->IsAlive()) continue;

        if (enemy->IsOnTrapTile())
        {
            enemy->TakeDamage(m_trapDamage);
        }
    }

    m_phaseEntered = false; // 전부 끝나면 다음 단계로.

    //std::cout << "moveDone" << std::endl;
    return true;
}

bool CombatController::EndPhase()
{
    //std::cout << "end" << std::endl;

    //PrintFrame();

    if (!m_phaseEntered)
    {
        m_phaseEntered = true;

        // 죽음 확정, 죽는 애니메이션 시작.
        if (m_aliceUnit && m_aliceUnit->IsAlive() && m_aliceUnit->GetHp() <= 0.0f)
        {
            m_aliceUnit->SetIsAlive(false);
            m_aliceUnit->SetAction(TurnAction::Die);
            //m_aliceUnit->SetActionDone(false);
            //m_aliceUnit->StartAction();
            //m_aliceUnit->StartAliceDieAnim();
            m_stageResult = StageResult::Lose; // 앨리스 죽으면 패배
        }

        for (AllyUnit* ally : m_allyUnits)
        {
            if (!ally || !ally->IsAlive()) continue;

            if (ally->GetHp() <= 0.0f)
            {
                ally->SetIsAlive(false);
                ally->SetAction(TurnAction::Die);
                //ally->SetActionDone(false);
                //ally->StartAction();
                ally->StartDieAnim();
            }
        }

        for (EnemyUnit* enemy : m_enemyUnits)
        {
            if (!enemy || !enemy->IsAlive()) continue;

            if (enemy->GetHp() <= 0.0f)
            {
                enemy->SetIsAlive(false);
                enemy->SetAction(TurnAction::Die);
                //enemy->SetActionDone(false);
                //enemy->StartAction();
                enemy->StartDieAnim();
                if (!m_redQueenUnit && enemy->IsBoss()) m_stageResult = StageResult::Win; // 보스 잡으면 무조건 승리
            }
        }

        if (m_redQueenUnit && m_redQueenUnit->IsAlive() && m_redQueenUnit->GetHp() <= 0.0f)
        {
            m_redQueenUnit->SetIsAlive(false);
            m_redQueenUnit->SetAction(TurnAction::Die);
            //m_redQueenUnit->SetActionDone(false);
            //m_redQueenUnit->StartAction();
            //m_redQueenUnit->StartQueenDieAnim();
            m_stageResult = StageResult::Win; // 붉은여왕 죽으면 승리
        }

        int aliveAllyCount = 0;
        for (AllyUnit* ally : m_allyUnits)
        {
            if (ally && ally->IsAlive()) ++aliveAllyCount;
        }

        if (aliveAllyCount == 0 && m_stageResult != StageResult::Win) m_stageResult = StageResult::Lose; // 전멸하면 패배


        if (m_redQueenUnit && m_redQueenUnit->IsAlive())
        {
            int aliveEnemyCount = 0;
            for (EnemyUnit* enemy : m_enemyUnits)
            {
                if (enemy && enemy->IsAlive()) ++aliveEnemyCount;
            }
            
            int movableAllyCount = 0;
            for (AllyUnit* ally : m_allyUnits)
            {
                if (ally && ally->IsAlive() && ally->GetMoveRule() != MoveRule::Hold) ++movableAllyCount;
            }

            // 붉은 여왕이 살아있는데 다른 적은 다 죽었고, 살아있는 아군이 전부 대기면 패배.
            if (aliveEnemyCount == 0 && movableAllyCount == 0 && m_stageResult != StageResult::Win) m_stageResult = StageResult::Lose;
        }

        return false;
    }   

    // 애니메이션 완료 체크
    
    //if (m_aliceUnit->IsAnimStart() && !m_aliceUnit->IsAnimDone()) return false;
    //if (m_redQueenUnit->IsAnimStart() && !m_redQueenUnit->IsAnimDone()) return false;

    //if (m_aliceUnit && !m_aliceUnit->IsAlive())
    //{
    //    if (!m_aliceUnit->IsActionDone()) return false;
    //    if (m_aliceUnit->IsAnimStart() && !m_aliceUnit->IsAnimDone()) return false;
    //}

    //for (AllyUnit* ally : m_allyUnits)
    //{
    //    if (!ally || ally->IsAlive()) continue;

    //    if (!ally->IsActionDone()) return false;
    //    if (ally->IsAnimStart() && !ally->IsAnimDone()) return false;
    //}

    //for (EnemyUnit* enemy : m_enemyUnits)
    //{
    //    if (!enemy || enemy->IsAlive()) continue;

    //    if (!enemy->IsActionDone()) return false;
    //    if (enemy->IsAnimStart() && !enemy->IsAnimDone()) return false;
    //}

    // 리셋
    for (AllyUnit* ally : m_allyUnits)
    {
        if (ally) ally->ResetTurnPlan();
    }

    for (EnemyUnit* enemy : m_enemyUnits)
    {
        if (enemy) enemy->ResetTurnPlan();
    }

    // 종료조건 만족시 승패 처리
    if (m_stageResult != StageResult::InProgress && !m_stageEnd)
    {
        if (m_healskillButton && m_attackskillButton)
        {
            // 이 경우 uibutton 가져와서 꺼주기.
            m_healskillButton->GetComponent<UIButton>()->SetActive(false);
            m_attackskillButton->GetComponent<UIButton>()->SetActive(false);
        }
        //튜토리얼 특수처리
        //Cat_Explain_EnemyRange 인 경우에 nextstep(true)호출
        Scene* scene = SceneManager::Instance().GetActiveScene();
        if (scene)
        {
            if (scene->GetName() == "TutorialScene" || scene->GetName() == "EndingScene") {
                GameObject* go = GameObject::Find("TutorialManager");

                if (!go) return false;

                TutorialManager* tutorialMgr = go->GetComponent<TutorialManager>();
                if (tutorialMgr)
                {
                    if (tutorialMgr->GetCurrentStep() == TutorialManager::TutorialStep::Battle)
                    {
                        tutorialMgr->NextStep(true);
                    }
                }

                return false;
            }
        }

        m_stageEnd = true;

        SoundManager::Instance().StopBGM(true);

        if (m_stageResult == StageResult::Win)
        {
            //std::cout << "Win!!!!!!!!!!!!" << std::endl;
            if (m_redQueenUnit) m_redQueenUnit->StartQueenDieAnim();
            
            for (AllyUnit* ally : m_allyUnits)
            {
                if (ally && ally->IsAlive()) ally->StartIdleAnim();
            }

            for (EnemyUnit* enemy : m_enemyUnits)
            {
                if (enemy && enemy->IsAlive()) enemy->StartDieAnim();
            }

            if (m_victoryBG && m_loseBG)
            {
                auto value = m_victoryBG->GetComponent<BgFadeController>();
                if (value)
                    value->SetFadeType(FadeType::Victory);

                m_victoryBG->SetActive(true);
                m_loseBG->SetActive(false);
                if(m_overBG)
                    m_overBG->SetActive(false);
            }
            
            // 승리 처리 이거 두개 다 clicknextstageevent에서 처리. 
            //GameManager::Instance()->NextStage(); 
            //ClientSceneManager::Instance().LoadScene("MainGameScene");

            // winUI 안띄우고 임시로 처리중 예제코드
        }
        else if (m_stageResult == StageResult::Lose)
        {
            //std::cout << "Lose!!!!!!!!!!!!" << std::endl;
            if (m_aliceUnit) m_aliceUnit->StartAliceDieAnim();

            // 패배 처리
            for (AllyUnit* ally : m_allyUnits)
            {
                if (ally && ally->IsAlive()) ally->StartDieAnim();
            }

            for (EnemyUnit* enemy : m_enemyUnits)
            {
                if (enemy && enemy->IsAlive()) enemy->StartIdleAnim();
            }

            //GameManager::Instance()->ShowLoseWindow();
            if (m_victoryBG && m_loseBG && m_overBG)
            {
                // 지는 경우.
                bool isGameOver = (GameManager::Instance()->GetLife() <= 1);

                m_victoryBG->SetActive(false);
                m_loseBG->SetActive(false);
                m_overBG->SetActive(false);

                if (isGameOver)
                {
                    //std::cout << "게임오버임." << std::endl;
                    if (auto value = m_overBG->GetComponent<BgFadeController>())
                    {
                        //std::cout << "게임오버 type 설정." << std::endl;
                        value->SetFadeType(FadeType::GameOver);
                    }
                    m_overBG->SetActive(true); 
                }
                else
                {
                    if (auto value = m_loseBG->GetComponent<BgFadeController>())
                    {
                        //std::cout << "패배." << std::endl;
                        value->SetFadeType(FadeType::Defeat);
                        //std::cout << "패배 type 설정." << std::endl;
                    }
                    m_loseBG->SetActive(true);
                }
            }

         
            //ClientSceneManager::Instance().LoadScene("MainGameScene"); 이것도 clickretry에서 처리중.
        }

    }

    m_phaseEntered = false;

    //std::cout << "endDone" << std::endl;
    return true;
}

void CombatController::Process(float dTime)
{
    if (m_stageEnd || !m_isStageStart) return;

    switch (m_currPhase)
    {
    case Phase::Ready:
        if (ReadyPhase()) m_currPhase = Phase::ReadyToMove;
        break;
    case Phase::ReadyToMove:
        if (ReadyToMovePhase()) m_currPhase = Phase::ReadyForBattle;
        break;
    case Phase::ReadyForBattle:
        if (ReadyForBattlePhase()) m_currPhase = Phase::MoveAndBattle;
        break;
    case Phase::MoveAndBattle:
        if (MoveAndBattlePhase(dTime)) m_currPhase = Phase::End; // 여기서 false면 계속 MoveAndBattle 유지
        break;
    case Phase::End:
        if (EndPhase()) m_currPhase = Phase::Ready; // 여기서 false면 계속 End 유지
        break;
    }
}

bool CombatController::CanActuallyAttack(const Unit* me, const Unit* target) const
{
    if (!me || !target) return false;
    if (!me->IsAlive() || !target->IsAlive()) return false;

    int a = me->GetStats().attackRange;

    if (!battleGrid->IsInRange(me->GetPos(), target->GetPos(), a)) return false;

    return battleGrid->HasLineOfSight(me->GetPos(), target->GetPos());
}

void CombatController::GetAttackableTargets(Unit* me, std::vector<Unit*>& outTargets) const
{
    outTargets.clear();
    if (!me || !me->IsAlive()) return;

    if (me->GetTeam() == Team::Ally)
    {
        for (EnemyUnit* enemy : m_enemyUnits)
        {
            if (enemy && CanActuallyAttack(me, enemy)) outTargets.push_back(enemy);
        }
        if (m_redQueenUnit && CanActuallyAttack(me, m_redQueenUnit)) outTargets.push_back(m_redQueenUnit);
    }
    else
    {
        for (AllyUnit* ally : m_allyUnits)
        {
            if (ally && CanActuallyAttack(me, ally)) outTargets.push_back(ally);
        }
        if (m_aliceUnit && CanActuallyAttack(me, m_aliceUnit)) outTargets.push_back(m_aliceUnit);
    }
}

static int DirDiff8(int a, int b)
{
    int d = std::abs(a - b);
    return (std::min)(d, 8 - d); // 0 ~ 4, 0에 가까울수록 우선순위
}

static bool HasTypeAdvantage(int me, int target)
{
    // 일단 나 > 비 > 룩 > 나
    return  (me == UnitType::Knight && target == UnitType::Bishop) ||
        (me == UnitType::Bishop && target == UnitType::Rook) ||
        (me == UnitType::Rook && target == UnitType::Knight);
}

static int GetTypeAdvantage(int me, int target) // 내가 상성우위면 1, 상대가 상성우위면 -1, 같으면 0
{
    if (HasTypeAdvantage(me, target)) return 1;
    if (HasTypeAdvantage(target, me)) return -1;
    return 0;
}

Unit* CombatController::SelectAttackTarget(Unit* me, const std::vector<Unit*>& targets, int battleRule) const
{
    if (!me || targets.empty()) return nullptr; // 공격 가능한 상대가 없음.
    if (targets.size() == 1) return targets[0]; // 공격 가능한 상대가 하나뿐이면 걔 공격하면 됨. 간단.. 

    int rule = battleRule;

    Unit* best = nullptr;

    int bestDirDiff = 9999; // 시선 우선순위를 위해. 얘는 공통.
    int bestDist = 9999;    // 가장 가까운 상대 거리. 
    float bestHp = 9999.9f; // 가장 체력 낮은 적.
    int bestAdv = -1;       // 상성 우위인 적.

    int meDir = (int)me->GetDir();
    Vector2 mePos = me->GetPos();
    int meType = me->GetUnitType();

    for (Unit* target : targets)
    {
        if (!target) continue;
        if (target == me->GetAttackTarget()) return target;

        Vector2 tp = target->GetPos();

        int toDir = me->GetDir2(tp);
        int dirDiff = DirDiff8(meDir, toDir);

        int dx = std::abs((int)tp.x - (int)mePos.x);
        int dy = std::abs((int)tp.y - (int)mePos.y);
        int dist = (std::max)(dx, dy); // 걍 맨해튼 말고 체비쇼프로 하자..

        float hp = target->GetHp();

        //bool adv = HasTypeAdvantage(meType, target->GetUnitType());
        int adv = GetTypeAdvantage(meType, target->GetUnitType());

        bool better = false;

        switch (rule)
        {
        case BattleRule::Nearest:
            if (dist < bestDist) better = true;
            else if (dist == bestDist && dirDiff < bestDirDiff) better = true;
            break;

        case BattleRule::LowestHp:
            if (hp < bestHp) better = true;
            else if (hp == bestHp)
            {
                if (dist < bestDist) better = true;
                else if (dist == bestDist && dirDiff < bestDirDiff) better = true;
            }
            break;

        case BattleRule::TypeAdvantage:
            if (adv > bestAdv) better = true;
            else if (adv == bestAdv)
            {
                if (dist < bestDist) better = true;
                else if (dist == bestDist && dirDiff < bestDirDiff) better = true;
            }
            break;
        }

        if (!best || better)
        {
            best = target;

            bestDist = dist;
            bestDirDiff = dirDiff;

            bestHp = hp;

            bestAdv = adv;
        }
    }

    return best;
}

bool CombatController::IsBattleUnit(const Unit* u) const
{
    return std::find(m_battleUnits.begin(), m_battleUnits.end(), u) != m_battleUnits.end();
}

static int DistOctileCost(const Vector2& a, const Vector2& b)
{
    int dx = std::abs((int)b.x - (int)a.x);
    int dy = std::abs((int)b.y - (int)a.y);
    int dmin = (std::min)(dx, dy);
    int dmax = (std::max)(dx, dy);
    return 14 * dmin + 10 * (dmax - dmin);
}

Unit* CombatController::FindNearestEnemy(const Vector2& from) const
{
    /*EnemyUnit* best = nullptr;
    int bestDist = 9999;

    for (EnemyUnit* enemy : m_enemyUnits)
    {
        if (!enemy || !enemy->IsAlive()) continue;
        int dx = std::abs((int)enemy->GetPos().x - (int)from.x);
        int dy = std::abs((int)enemy->GetPos().y - (int)from.y);
        int dist = (std::max)(dx, dy);
        if (dist < bestDist) { bestDist = dist; best = enemy; }
    }
    return best;*/

    Unit* best = nullptr;
    int bestCost = 999999;
    int bestCheb = 999999; // 동점 깨기용(선택)

    // 일반 적들
    for (EnemyUnit* enemy : m_enemyUnits)
    {
        if (!enemy || !enemy->IsAlive()) continue;

        Vector2 p = enemy->GetPos();

        int cost = DistOctileCost(p, from);
        int dx = std::abs((int)p.x - (int)from.x);
        int dy = std::abs((int)p.y - (int)from.y);
        int cheb = (std::max)(dx, dy);

        if (!best || cost < bestCost || (cost == bestCost && cheb < bestCheb))
        {
            best = enemy;
            bestCost = cost;
            bestCheb = cheb;
        }
    }

    // 붉은 여왕
    if (m_redQueenUnit && m_redQueenUnit->IsAlive())
    {
        Vector2 p = m_redQueenUnit->GetPos();

        int cost = DistOctileCost(p, from);
        int dx = std::abs((int)p.x - (int)from.x);
        int dy = std::abs((int)p.y - (int)from.y);
        int cheb = (std::max)(dx, dy);

        if (!best || cost < bestCost || (cost == bestCost && cheb < bestCheb))
        {
            best = m_redQueenUnit;
            bestCost = cost;
            bestCheb = cheb;
        }
    }


    return best;
}

Unit* CombatController::FindNearestAlly(const AllyUnit* me, const Vector2& from) const
{
    AllyUnit* best = nullptr;
    int bestDist = 9999;

    for (AllyUnit* ally : m_allyUnits)
    {
        if (!ally || !ally->IsAlive() || ally == me) continue;
        if (ally->GetMoveRule() == MoveRule::Hold) continue; // 대기 아군은 제외

        int dx = std::abs((int)ally->GetPos().x - (int)from.x);
        int dy = std::abs((int)ally->GetPos().y - (int)from.y);
        int dist = (std::max)(dx, dy);
        if (dist < bestDist) { bestDist = dist; best = ally; }
    }
    return best;
}

Vector2 CombatController::DecideMoveTarget_Ally(AllyUnit* me) const
{
    Vector2 mePos = me->GetPos();
    Vector2 myMovePos = me->GetMovePos();
    int rule = me->GetMoveRule();

    if (rule == MoveRule::Chase) // 추격
    {
        if (m_redQueenUnit && m_redQueenUnit->IsAlive())
        {
            me->SetMoveTarget(m_redQueenUnit);
            return m_redQueenUnit->GetPos();
        }
        
        for (EnemyUnit* enemy : m_enemyUnits)
        {
            if (enemy && enemy->IsAlive() && enemy->IsBoss())
            {
                me->SetMoveTarget(enemy);
                return enemy->GetPos();
            }
        }
    }

    if (rule == MoveRule::Hold) // 대기
    {
        Vector2 defPos = GRIDPOS_INVALID;
        if (battleGrid->GetNearestDefenseTile(mePos, myMovePos, defPos))
        { 
            me->SetMoveTarget(nullptr);
            return defPos;
        }
        return GRIDPOS_INVALID; // 방어타일 없으면 목표 없음 처리
    }


    // 전진
    bool allyInPerception = false; // 인식범위 내 아군이 있는지. 
    int aliveAllyCount = 0; // 스테이지 내 본인이 아닌 아군의 수
    int pr = me->GetPerceptionRange(); // 인식범위

    for (AllyUnit* ally : m_allyUnits)
    {
        if (!ally || !ally->IsAlive()) continue;
        if (ally == me) continue;
        if (ally->GetMoveRule() == MoveRule::Hold) continue; // 대기 아군은 제외

        ++aliveAllyCount;
        if (battleGrid->IsInRange(mePos, ally->GetPos(), pr)) allyInPerception = true; 
    }

    Unit* target = nullptr;

    if (aliveAllyCount == 0)
    {
        target = FindNearestEnemy(mePos);
        if (!target) return GRIDPOS_INVALID;

        me->SetMoveTarget(target);
        return target->GetPos(); // 아군이 혼자뿐이면 적을 향해 간다. 
    }

    if (!allyInPerception)
    {
        target = FindNearestAlly(me, mePos);
        if (!target)
        {
            target = FindNearestEnemy(mePos);
            if (!target) return GRIDPOS_INVALID;
        }


        me->SetMoveTarget(target);
        return target->GetPos(); // 아군이 인식범위 안에 없으면 아군을 향해 간다. 
    }

    target = FindNearestEnemy(mePos);
    if (!target) return GRIDPOS_INVALID;

    me->SetMoveTarget(target);
    return target->GetPos();
}

Vector2 CombatController::DecideMovePos_Ally(AllyUnit* me, const Vector2& moveTarget)
{
    if (moveTarget == GRIDPOS_INVALID) return me->GetPos();

    int rule = me->GetMoveRule();
    Vector2 next;

    //bool found = battleGrid->FindNextStepAStar(me->GetPos(), moveTarget, me, me->GetMoveTarget(), rule, next);
    //if (!found)
    //{
    //    found = battleGrid->FindNextStepGreedy(me->GetPos(), moveTarget, me, me->GetMoveTarget(), rule, next);
    //}

    //if (!found) return me->GetPos();

    bool found = battleGrid->FindNextStepHybrid(me->GetPos(), moveTarget, me, me->GetMoveTarget(), rule, next);
    if (!found) return me->GetPos();

    // 추격을 선택했을 때 가는 칸이 부벽이면 이동 대신 벽을 공격
    if (rule == MoveRule::Chase && battleGrid->IsBreakableWall(next))
    {
        me->SetMoveTargetPos(GRIDPOS_INVALID);
        me->SetAttackTarget(nullptr/*나중에 부서지는 벽 만들면 넣어줄게..*/); // 안해도 될듯..?
        me->SetAttackPos(next);
        m_battleUnits.push_back(me);

        return GRIDPOS_INVALID;
    }

    return next;
}

static int Sign(int x)
{
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}


Vector2 CombatController::DecideMovePos_Enemy(EnemyUnit* me)
{
    Vector2 curPos = me->GetPos();

    int idx = me->GetPathIndex();
    Vector2 goal = me->GetPathPoint(idx);

    while (idx < 7 && goal != GRIDPOS_INVALID && curPos == goal)
    {
        idx++;
        me->SetPathIndex(idx);
        goal = me->GetPathPoint(idx);
    }

    if (goal == GRIDPOS_INVALID) return curPos;

    int dx = (int)goal.x - (int)curPos.x;
    int dy = (int)goal.y - (int)curPos.y;

    int sx = Sign(dx);
    int sy = Sign(dy);

    Vector2 next{ curPos.x + sx, curPos.y + sy };

    if (battleGrid->IsBlocked(next, me, nullptr, MoveRule::Advance)) return curPos;

    return next;
}

static Vector2 GetMidPosForRange2(const Vector2& me, const Vector2& target)
{ // 어차피 원거리 사거리 2니까 쉽게쉽게 가자. 딱 브레젠험 한번만 하면 됨.
    if (me == GRIDPOS_INVALID || target == GRIDPOS_INVALID) return GRIDPOS_INVALID;

    int x0 = (int)me.x;
    int y0 = (int)me.y;
    int x1 = (int)target.x;
    int y1 = (int)target.y;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    // 2칸 떨어진 경우만 이걸 하면 됨. 근거리 공격일 경우는 생각 안함.
    int dist = (std::max)(dx, dy);
    if (dist != 2) return GRIDPOS_INVALID;

    // L자 케이스는 중간칸 없는거로.
    if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) return GRIDPOS_INVALID;

    // 직선, 완전대각은 mid가 확정
    int sx = (x1 > x0) ? 1 : (x1 < x0 ? -1 : 0);
    int sy = (y1 > y0) ? 1 : (y1 < y0 ? -1 : 0);

    return Vector2{ (float)(x0 + sx), (float)(y0 + sy) };

    //int sx = Sign(x1 - x0);
    //int sy = Sign(y1 - y0);

    //if (dx == 2 && dy == 0) { return Vector2{ (float)(x0 + sx), (float)y0 }; }
    //if (dx == 0 && dy == 2) { return Vector2{ (float)x0, (float)(y0 + sy) }; }
    //if (dx == 2 && dy == 2) { return Vector2{ (float)(x0 + sx), (float)(y0 + sy) }; }

    // 완전대각이 아닌 애매한 케이스는 브레젠험 딱 한번만 해서 mid 구함.
    //int sx2 = (x0 < x1) ? 1 : -1;
    //int sy2 = (y0 < y1) ? 1 : -1;
    //int err = dx - dy;

    //int e2 = err * 2;
    //if (e2 > -dy) { err -= dy; x0 += sx2; }
    //if (e2 < dx) { err += dx; y0 += sy2; }

    //return Vector2{ (float)x0, (float)y0 };
}


Unit* CombatController::FindBlockingUnitByPos(const Vector2& pos, Unit* me) const
{
    if (!me) return nullptr;

    if (me->GetTeam() == Team::Ally)
    {
        for (EnemyUnit* enemy : m_enemyUnits)
        {
            if (!enemy || !enemy->IsAlive()) continue;
            if (enemy->GetMovePos() == GRIDPOS_INVALID) continue;
            if (enemy->GetMovePos() == pos) return enemy;
        }
    }
    else
    {
        for (AllyUnit* ally : m_allyUnits)
        {
            if (!ally || !ally->IsAlive()) continue;
            if (ally->GetMovePos() == GRIDPOS_INVALID) continue;
            if (ally->GetMovePos() == pos) return ally;
        }
        if (m_aliceUnit && m_aliceUnit->GetMovePos() == pos) return m_aliceUnit;
    }

    return nullptr;
}


Vector2 CombatController::DecideAttackPos(Unit* me)
{
    if (!me) return GRIDPOS_INVALID;

    Unit* target = me->GetAttackTarget();

    if (!target)
    {
        me->SetAction(TurnAction::BreakWall);
        return me->GetAttackPos();
    }

    int a = me->GetStats().attackRange;
    if (battleGrid->IsInRange(me->GetPos(), target->GetMovePos(), a))
    {
        Vector2 mid = GetMidPosForRange2(me->GetPos(), target->GetMovePos());

        if (mid != GRIDPOS_INVALID)
        {
            Unit* newTarget = FindBlockingUnitByPos(mid, me);
            if (newTarget)
            {
                me->SetAttackTarget(newTarget);
                return newTarget->GetMovePos();
            }
        }
        return target->GetMovePos();
    }
    else
    {
        me->SetAction(TurnAction::Miss); // 빗나감 설정. 
        //std::cout << "빗나감\n";
        return target->GetPos();
    }
}

void CombatController::ResolveTurnAction(Unit* me)
{
    if (!me || !me->IsAlive()) return;

    switch (me->GetAction()) {
    case TurnAction::Wait: // 대기 처리
    {
        me->StartIdleAnim();
    } break;

    case TurnAction::Move: // 이동 처리
    {
        me->SetDir(me->GetMovePos());
        me->SetPos(me->GetMovePos());

        //me->StartMoveAnim();
    } break;

    case TurnAction::Attack: // 공격 처리
    {
        Unit* target = me->GetAttackTarget();
        if (target && target->IsAlive())
        {
            float damage = CalculateDamage(me, target);

            me->SetDir(me->GetAttackPos());
            target->TakeDamage(damage);


            if (me->GetUnitType() == UnitType::Bishop)
                EffectManager::Instance().PlayEffect("2Attack03", target->_GetOwner());

            //me->StartAttackAnim();
        }

    }  break;

    case TurnAction::Miss: // 빗나감 처리
    {
        me->SetDir(me->GetAttackPos());

        //me->StartAttackAnim();
    }  break;

    case TurnAction::BreakWall: // 벽 파괴 처리
    {
        me->SetDir(me->GetAttackPos());
        battleGrid->WallBreak(me->GetAttackPos());

        //me->StartAttackAnim();
    } break;

    default:
        return;
    }
}

void CombatController::ApplyActionResult(Unit* me)
{
    if (!me || !me->IsAlive() || me->IsActionResultApplied()) return;

    switch (me->GetAction())
    {
    case TurnAction::Attack:
    {
        Unit* target = me->GetAttackTarget();

        if (target && target->IsAlive())
        {
            float damage = CalculateDamage(me, target);
            target->TakeDamage(damage);


           
        }
        me->SetActionResultApplied(true);
    } break;

    case TurnAction::BreakWall:
    {
        battleGrid->WallBreak(me->GetAttackPos());
        me->SetActionResultApplied(true);
    } break;

    default:
        me->SetActionResultApplied(true);
        break;
    }
}

float CombatController::CalculateDamage(Unit* me, Unit* target)
{
    float bonus = 1.0f; // 상성 보너스.
    if (HasTypeAdvantage(me->GetUnitType(), target->GetUnitType())) bonus = 1.5f;
    else if (HasTypeAdvantage(target->GetUnitType(), me->GetUnitType())) bonus = 0.7f;

    float bonus2 = 1.0f; // 방어타일 보너스.
    if (battleGrid->IsDefenseTile(target->GetMovePos())) bonus2 = 1.5f;

    float damage = ((float)me->GetStats().atk - (float)target->GetStats().def * bonus2) * bonus;

    //std::cout << damage << std::endl;

    return (std::max)(damage, 0.0f);
}


void CombatController::AddAllyUnit(AllyUnit* allyUnit)
{
    if (allyCount == 0) allyUnit0 = allyUnit;
    else if (allyCount == 1) allyUnit1 = allyUnit;
    else if (allyCount == 2) allyUnit2 = allyUnit;
    else return;

    ++allyCount;
}

void CombatController::AddEnemyUnit(EnemyUnit* enemyUnit)
{
    if (enemyCount == 0) enemyUnit0 = enemyUnit;
    else if (enemyCount == 1) enemyUnit1 = enemyUnit;
    else if (enemyCount == 2) enemyUnit2 = enemyUnit;
    else return;

    ++enemyCount;
}

void CombatController::PrintFrame()
{
    if (!battleGrid) return;

    auto isSame = [](const Vector2& a, const Vector2& b) {
        return (int)a.x == (int)b.x && (int)a.y == (int)b.y;
        };

    for (int y = 0; y < battleGrid->GetHeight(); ++y)
    {
        for (int x = 0; x < battleGrid->GetWidth(); ++x)
        {
            Vector2 p{ (float)x, (float)y };
            const auto& s = battleGrid->GetStaticTile(p);
            const auto& d = battleGrid->GetDynamicTile(p);

            char c = ' ';
            if (!s.tile) c = ' ';
            else if (s.solidWall) c = '#';
            else if (s.breakableWall) c = '*';
            else if (s.defenseTile) c = 'D';
            else if (s.trapTile) c = 'T';
            else c = '.';

            if (d.reservedMove && !d.unitPresent)
            {
                c = 'X';
            }

            if (m_aliceUnit && m_aliceUnit->IsAlive() && isSame(m_aliceUnit->GetPos(), p)) c = 'S';
            if (m_redQueenUnit && m_redQueenUnit->IsAlive() && isSame(m_redQueenUnit->GetPos(), p)) c = 'R';

            for (auto* a : m_allyUnits)
                if (a && a->IsAlive() && isSame(a->GetPos(), p)) c = 'A';

            for (auto* e : m_enemyUnits)
                if (e && e->IsAlive() && isSame(e->GetPos(), p)) c = (e->IsBoss() ? 'B' : 'E');

            //std::cout << c << ' ';
        }
       // std::cout << "\n";
    }

    auto printUnit = [](const char* name, const Unit* u)
        {
            if (!u) { std::cout << name << ": (null)\n"; return; }
            std::cout << name
                << " pos(" << (int)u->GetPos().x << "," << (int)u->GetPos().y << ")"
                << " hp=" << u->GetHp()
                << " alive=" << (u->IsAlive() ? 1 : 0)
                << " act=" << (int)u->GetAction()
                << " done=" << (u->IsActionDone() ? 1 : 0)
                << " move(" << (int)u->GetMovePos().x << "," << (int)u->GetMovePos().y << ")"
                << " movetarget(" << (int)u->GetMoveTargetPos().x << "," << (int)u->GetMoveTargetPos().y << ")"
                << " atkPos(" << (int)u->GetAttackPos().x << "," << (int)u->GetAttackPos().y << ")"
                << "\n";
        };

    if (m_aliceUnit) printUnit("Alice", m_aliceUnit);
    if (allyUnit0) printUnit("Ally0", allyUnit0);
    if (allyUnit1) printUnit("Ally1", allyUnit1);
    if (allyUnit2) printUnit("Ally2", allyUnit2);
    if (enemyUnit0) printUnit("Enemy0", enemyUnit0);
    if (enemyUnit1) printUnit("Enemy1", enemyUnit1);
    if (enemyUnit2) printUnit("Enemy2", enemyUnit2);
    if (m_redQueenUnit) printUnit("Queen", m_redQueenUnit);

    //auto dumpCell = [&](int x, int y)
    //    {
    //        Vector2 p{ (float)x, (float)y };
    //        const auto& s = battleGrid->GetStaticTile(p);
    //        const auto& d = battleGrid->GetDynamicTile(p);

    //        std::cout << "[Cell " << x << "," << y << "] "
    //            << "tile=" << s.tile
    //            << " solid=" << s.solidWall
    //            << " break=" << s.breakableWall
    //            << " def=" << s.defenseTile
    //            << " trap=" << s.trapTile
    //            << " unitPresent=" << d.unitPresent
    //            << " reserved=" << d.reservedMove
    //            << "\n";
    //    };

    //dumpCell(1, 6);
    //dumpCell(0, 5);
    //dumpCell(1, 5); // Ally2 현재칸
}