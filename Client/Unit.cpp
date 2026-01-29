#include "Unit.h"

static const UnitStats UnitStatsTable[] =
{
    // 공, 방, 체, 공격범위
    { 20, 10, 150, 1 }, // 룩
    { 25,  5, 110, 1 }, // 나이트
    { 30,  2,  90, 2 }, // 비숍
    {  0,  0, 150, 0 }, // 앨리스
    //{ 30,  3, 300, 1 }, // 퀸
};

void Unit::SetUnitType(int type)
{
    if (type < UnitType::Rook || type > UnitType::Alice) type = UnitType::Rook;
    m_type = type;
    SetStats(UnitStatsTable[type]);
}

void Unit::SetStats(const UnitStats& s)
{
    m_stats = s;
    m_hp = m_stats.maxHp;
}

static Vector3 GridToWorld(const Vector2& p)
{
    return Vector3{ p.x * 2.0f, 1.0f, p.y * 2.0f };
}

void Unit::StartAction()
{   
    if (m_action == TurnAction::Move)
    {
        m_worldFrom = _GetOwner()->GetTransform()->GetPosition();
        m_worldTo = GridToWorld(m_pos);

        m_moveT = 0.0f;
        m_isMoving = true;
        m_actionDone = false;
        return;
    }
    else if (m_action == TurnAction::Die)
    {
        _GetOwner()->GetTransform()->SetPosition(Vector3{ m_pos.x * 2.0f, -3, m_pos.y * 2.0f });
    }

    m_actionDone = true;
}

void Unit::UpdateAction(float dTime)
{
    if (!m_isMoving) return;

    m_moveT += dTime / m_moveDuration;
    float t = (m_moveT > 1.0f) ? 1.0f : m_moveT;

    float smooth = t * t * (3.0f - 2.0f * t);

    Vector3 p = m_worldFrom + (m_worldTo - m_worldFrom) * smooth;
    _GetOwner()->GetTransform()->SetPosition(p);

    if (t >= 1.0f)
    {
        m_isMoving = false;
        m_actionDone = true;
    }
}

int Unit::GetDir2(const Vector2& p) const // 대충 어느 위치 던져주면 그 위치를 바라보는 방향을 8방향으로 바꿔줌.
{
    float vx = p.x - m_pos.x;
    float vy = p.y - m_pos.y;

    if (vx == 0.0f && vy == 0.0f) return (int)m_dir; // 내 위치면 리턴.

    float ang = std::atan2(-vy, vx); // 각도 구하기. 위가 +y로 맞춰주기 위해서 -vy함.

    //Right=0, UpRight=1, Up=2, UpLeft=3, Left=4, DownLeft=5, Down=6, DownRight=7
    float step = 3.1415926535f / 4.0f; // 360 / 8 = 45도
    int dir = std::lround(ang / step); // 몇 번째 45도 칸?
    dir = (dir % 8 + 8) % 8; // 0 ~ 7 로 끼워넣기. 음수도.

    return dir;
}

void Unit::SetDir(const Vector2& p) 
{
    m_dir = static_cast<Dir8>(GetDir2(p));
}

void Unit::ResetTurnPlan()
{
    m_moveTarget = nullptr;
    m_moveTargetPos = GRIDPOS_INVALID;
    m_movePos = GRIDPOS_INVALID;
    m_attackTarget = nullptr;
    m_attackPos = GRIDPOS_INVALID;
    m_action = TurnAction::Wait;
    if (m_isAlive) m_actionDone = false;
    else m_actionDone = true;
}