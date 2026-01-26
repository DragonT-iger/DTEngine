#include "CombatController.h"
#include "GameObject.h"

BEGINPROPERTY(CombatController)
DTPROPERTY_SETTER(CombatController, battleGrid, SetBattleGrid)
DTPROPERTY_SETTER(CombatController, allyUnit0, SetAllyUnit0)
DTPROPERTY_SETTER(CombatController, allyUnit1, SetAllyUnit1)
DTPROPERTY_SETTER(CombatController, allyUnit2, SetAllyUnit2)
DTPROPERTY_SETTER(CombatController, enemyUnit0, SetEnemyUnit0)
DTPROPERTY_SETTER(CombatController, enemyUnit1, SetEnemyUnit1)
DTPROPERTY_SETTER(CombatController, enemyUnit2, SetEnemyUnit2)
ENDPROPERTY()


CombatController::~CombatController()
{
    battleGrid = nullptr;

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
    Setup();
}

void CombatController::Update(float dTime)
{
    //Process();
}

void CombatController::Setup()
{
    if (allyUnit0) m_allyUnits.push_back(allyUnit0);
    if (allyUnit1) m_allyUnits.push_back(allyUnit1);
    if (allyUnit2) m_allyUnits.push_back(allyUnit2);

    if (enemyUnit0) m_enemyUnits.push_back(enemyUnit0);
    if (enemyUnit1) m_enemyUnits.push_back(enemyUnit1);
    if (enemyUnit2) m_enemyUnits.push_back(enemyUnit2);
}

bool CombatController::ReadyPhase()
{
    battleGrid->ClearDynamicGrid();
    battleGrid->SyncUnitsPos(m_allyUnits, m_enemyUnits);

    for (AllyUnit* ally : m_allyUnits)
    {
        if (!ally || !ally->IsAlive()) continue;

        std::vector<Unit*> targets;
        GetAttackableTargets(ally, targets);

        if (!targets.empty())
        {
            m_battleUnits.push_back(ally);

            Unit* target = SelectAttackTarget(ally, targets);
            ally->SetAttackTarget(target);
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

            Unit* target = SelectAttackTarget(enemy, targets);
            enemy->SetAttackTarget(target);
        }
    }

    return true;
}

bool CombatController::ReadyToMovePhase()
{
	return false;
}

bool CombatController::ReadyForBattlePhase()
{
	return false;
}

bool CombatController::MoveAndBattlePhase()
{
	return false;
}

bool CombatController::EndPhase()
{
	return false;
}

void CombatController::Process()
{
    while (m_stageResult == StageResult::InProgress) 
    {
        switch (m_currPhase) {
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
            if (MoveAndBattlePhase()) m_currPhase = Phase::End;
            break;
        case Phase::End:
            if (EndPhase()) m_currPhase = Phase::Ready;
            break;
        }
    }

    if (m_stageResult == StageResult::Win) 
    {
        // 승리 처리
    }
    else if (m_stageResult == StageResult::Lose)
    {
        // 패배 처리
    }
}

bool CombatController::CanActuallyAttack(const Unit* me, const Unit* target) const
{
    if (!me || !target) return false;
    if (!me->IsAlive() || !target->IsAlive()) return false;

    int a = me->GetStats().attackRange;

    if (!battleGrid->IsInRange(me->GetPos(), target->GetPos(), a))
        return false;

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
            if (CanActuallyAttack(me, enemy)) outTargets.push_back(enemy);
        }
    }
    else
    {
        for (AllyUnit* ally : m_allyUnits)
        {
            if (CanActuallyAttack(me, ally)) outTargets.push_back(ally);
        }
    }
}

static int DirDiff8(int a, int b)
{
    int d = std::abs(a - b);
    return (std::min)(d, 8 - d); // 0 ~ 4, 0에 가까울수록 우선순위
}

Unit* CombatController::SelectAttackTarget(Unit* me, const std::vector<Unit*>& targets) const
{
    if (targets.empty()) return nullptr; // 공격 가능한 상대가 없음.
    if (targets.size() == 1) return targets[0]; // 공격 가능한 상대가 하나뿐이면 걔 공격하면 됨. 간단.. 

    // 일단 가장 가까운 상대. 
    Unit* best = nullptr;
    int bestDist = 9999;
    int bestDirDiff = 9999; // 시선 우선순위를 위해. 

    Vector2 mePos = me->GetPos();
    int meDir = (int)me->GetDir();

    for (Unit* target : targets)
    {
        Vector2 tp = target->GetPos();
        
        int dx = std::abs((int)tp.x - (int)mePos.x);
        int dy = std::abs((int)tp.y - (int)mePos.y);
        int dist = (std::max)(dx, dy); // 걍 맨해튼 말고 체비쇼프로 하자..

        int toDir = me->GetDir2(tp);
        int dirDiff = DirDiff8(meDir, toDir);

        if (dist < bestDist || (dist == bestDist && dirDiff < bestDirDiff))
        {
            bestDist = dist;
            bestDirDiff = dirDiff;
            best = target;
        }
    }

    return best;
}

bool CombatController::IsBattleUnit(const Unit* u) const
{
    return std::find(m_battleUnits.begin(), m_battleUnits.end(), u) != m_battleUnits.end();
}

Vector2 CombatController::FindNearestEnemyPos(const Vector2& from) const 
{
    EnemyUnit* best = nullptr;
    int bestDist = 9999;

    for (EnemyUnit* enemy : m_enemyUnits)
    {
        if (!enemy || !enemy->IsAlive()) continue;
        int dx = std::abs((int)enemy->GetPos().x - (int)from.x);
        int dy = std::abs((int)enemy->GetPos().y - (int)from.y);
        int dist = (std::max)(dx, dy);
        if (dist < bestDist) { bestDist = dist; best = enemy; }
    }
    return best ? best->GetPos() : GRIDPOS_INVALID;
}

Vector2 CombatController::FindNearestAllyPos(const AllyUnit* me, const Vector2& from) const
{
    AllyUnit* best = nullptr;
    int bestDist = 9999;

    for (AllyUnit* ally : m_allyUnits)
    {
        if (!ally || !ally->IsAlive() || ally == me) continue;
        int dx = std::abs((int)ally->GetPos().x - (int)from.x);
        int dy = std::abs((int)ally->GetPos().y - (int)from.y);
        int dist = (std::max)(dx, dy);
        if (dist < bestDist) { bestDist = dist; best = ally; }
    }
    return best ? best->GetPos() : GRIDPOS_INVALID;
}

Vector2 CombatController::DecideMoveTarget_Ally(AllyUnit* me) const
{
    Vector2 mePos = me->GetPos();
    int rule = me->GetMoveRule();

    if (rule == MoveRule::Chase) // 추격
    {
        for (EnemyUnit* enemy : m_enemyUnits)
        {
            if (enemy && enemy->IsAlive() && enemy->IsBoss()) return enemy->GetPos();
        }          
    }

    if (rule == MoveRule::Hold) // 대기
    {
        Vector2 defPos = GRIDPOS_INVALID;
        if (battleGrid->GetNearestDefenseTile(mePos, defPos)) return defPos;
        return GRIDPOS_INVALID; // 방어타일 없으면 목표 없음 처리
    }


    // 전진
    bool allyInPerception = false; // 인식범위 내 적이 있는지. 
    int aliveAllyCount = 0; // 스테이지 내 본인이 아닌 아군의 수
    int pr = me->GetPerceptionRange(); // 인식범위

    for (AllyUnit* ally : m_allyUnits)
    {
        if (!ally || !ally->IsAlive() || ally == me) continue;

        ++aliveAllyCount;
        if (battleGrid->IsInRange(mePos, ally->GetPos(), pr)) { allyInPerception = true; } 
    }
    
    for (AllyUnit* a : m_allyUnits) { 
        if (a && a->IsAlive()) ++aliveAllyCount; 
    }

    if (aliveAllyCount == 0)  return FindNearestEnemyPos(mePos); // 아군이 혼자뿐이면 적을 향해 간다. 

    if (!allyInPerception) return FindNearestAllyPos(me, mePos); // 아군이 인식범위 안에 

    return FindNearestEnemyPos(mePos);
}