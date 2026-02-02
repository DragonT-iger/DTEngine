#include "EnemyUnit.h"
#include "GameObject.h"

BEGINPROPERTY(EnemyUnit)
DTPROPERTY(EnemyUnit, path0)
DTPROPERTY(EnemyUnit, path1)
DTPROPERTY(EnemyUnit, path2)
DTPROPERTY(EnemyUnit, path3)
DTPROPERTY(EnemyUnit, path4)
DTPROPERTY(EnemyUnit, path5)
DTPROPERTY(EnemyUnit, path6)
DTPROPERTY(EnemyUnit, path7)
DTPROPERTY_ACCESSOR(EnemyUnit, m_type, GetUnitType, SetUnitType)
DTPROPERTY_ACCESSOR(EnemyUnit, m_isBoss, IsBoss, SetBoss)
DTPROPERTY_ACCESSOR(EnemyUnit, m_pos, GetPos, SetPos)
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

std::vector<Vector2> EnemyUnit::GetAllPath() const
{
    std::vector<Vector2> tiles;
    tiles.reserve(64);

    Vector2 cur = GetPos();

    int idx = m_pathIndex;
    Vector2 goal = GetPathPoint(idx);

    while (idx < 7 && goal != GRIDPOS_INVALID && cur == goal)
    {
        idx++;
        goal = GetPathPoint(idx);
    }

    if (goal == GRIDPOS_INVALID) return tiles;

    const int maxSteps = 512; // 안전장치. 무한루프 방지.

    for (int step = 0; step < maxSteps; ++step)
    {
        // goal 도착 → 다음 goal
        while (idx < 7 && goal != GRIDPOS_INVALID && cur == goal)
        {
            idx++;
            goal = GetPathPoint(idx);
        }

        if (goal == GRIDPOS_INVALID)
            break;

        Vector2 next = StepToward(cur, goal);

        tiles.push_back(next);
        cur = next;
    }

    return tiles;
}


