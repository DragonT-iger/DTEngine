#include "EnemyUnit.h"
#include "GameObject.h"

BEGINPROPERTY(EnemyUnit)
//DTPROPERTY(EnemyUnit, path0)
DTPROPERTY(EnemyUnit, path1)
DTPROPERTY(EnemyUnit, path2)
DTPROPERTY(EnemyUnit, path3)
DTPROPERTY(EnemyUnit, path4)
DTPROPERTY(EnemyUnit, path5)
DTPROPERTY(EnemyUnit, path6)
DTPROPERTY(EnemyUnit, path7)
DTPROPERTY_ACCESSOR(EnemyUnit, m_type, GetUnitType, SetUnitType)
DTPROPERTY_ACCESSOR(EnemyUnit, m_isBoss, IsBoss, SetBoss)
//DTPROPERTY_ACCESSOR(EnemyUnit, m_pos, GetPos, SetPos)
ENDPROPERTY()

void EnemyUnit::SetBoss(bool isBoss)
{
    m_isBoss = isBoss;
    if (isBoss)
    {
        //UnitStats bossStats = GetStats() * m_BossScale; // 스텟 뻥튀기
        //bossStats.attackRange = GetStats().attackRange; // 단 공격범위는 예외.
        UnitStats bossStats = GetStats();
        bossStats.maxHp = bossStats.maxHp * m_BossScale; // 체력만 뻥튀기?
        SetStats(bossStats);
    }
    else
    {
        SetUnitType(m_type); // 다시 원래 스텟으로 돌리기
    }
}

Vector2 EnemyUnit::GetPathPoint(int i) const
{
	switch (i)
	{
	case 0: return path0;
	case 1: return path1;
	case 2: return path2;
	case 3: return path3;
	case 4: return path4;
	case 5: return path5;
	case 6: return path6;
	case 7: return path7;
	default: return GRIDPOS_INVALID;
	}
}


void EnemyUnit::SetPath0()
{
    Vector3 pos = _GetOwner()->GetTransform()->GetPosition();

    Vector2 p0;
    p0.x = std::round(pos.x / 2.0f);
    p0.y = std::round(pos.z / 2.0f);

    m_pos = p0;
    path0 = p0;
}

static int Sign(int x)
{
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

static Vector2 StepToward(const Vector2& cur, const Vector2& goal)
{
    int dx = (int)goal.x - (int)cur.x;
    int dy = (int)goal.y - (int)cur.y;

    int sx = Sign(dx);
    int sy = Sign(dy);

    return Vector2{ cur.x + sx, cur.y + sy };
}

static float GetDirFromTo(const Vector2& from, const Vector2& to)
{
    float vx = to.x - from.x;
    float vy = to.y - from.y;

    if (vx == 0.0f && vy == 0.0f) return 0;

    float ang = std::atan2(-vy, vx);

    // Right=0, UpRight=1, Up=2, UpLeft=3, Left=4, DownLeft=5, Down=6, DownRight=7
    float step = 3.1415926535f / 4.0f;
    int dir = (int)std::lround(ang / step);
    dir = (dir % 8 + 8) % 8;

    return dir * 45.0f;
}

std::vector<PathPoint> EnemyUnit::GetAllPath() const
{
    std::vector<PathPoint> tiles;
    tiles.reserve(128);

    int startIdx = 0;

    int endIdx = 0;
    for (int i = 7; i >= 0; --i)
    {
        if (GetPathPoint(i) != GRIDPOS_INVALID)
        {
            endIdx = i;
            break;
        }
    }

    if (endIdx <= startIdx) return tiles;

    //Vector2 cur = GetPathPoint(startIdx);

    Vector3 pos = _GetOwner()->GetTransform()->GetPosition();
    Vector2 cur;
    cur.x = std::round(pos.x / 2.0f);
    cur.y = std::round(pos.z / 2.0f);

    const int maxSteps = 128; 

    for (int idx = startIdx + 1; idx <= endIdx; ++idx)
    {
        Vector2 goal = GetPathPoint(idx);

        if (goal == GRIDPOS_INVALID) break;

        for (int step = 0; step < maxSteps; ++step)
        {
            if (cur == goal) break;

            Vector2 next = StepToward(cur, goal);

            if ((idx != endIdx) || (next != goal))
            {
                float dir = GetDirFromTo(cur, next);
                Vector3 wNext{ next.x * 2.0f, 1.1f, next.y * 2.0f};
                tiles.push_back(PathPoint{ wNext, dir });
            }

            cur = next;
        }
    }

    return tiles;
}