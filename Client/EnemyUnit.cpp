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

// 화살표 관련 로직
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

static Vector3 GridToWorld_A(const Vector2& g)
{
    return Vector3{ g.x * 2.0f, 1.1f, g.y * 2.0f };
}

static int Sign(int x)
{
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

static Vector2 StepTowardGrid(const Vector2& cur, const Vector2& goal)
{
    int dx = (int)goal.x - (int)cur.x;
    int dy = (int)goal.y - (int)cur.y;
    return Vector2{ cur.x + (float)Sign(dx), cur.y + (float)Sign(dy) };
}

static int GridDist(const Vector2& a, const Vector2& b)
{
    int dx = std::abs((int)b.x - (int)a.x);
    int dy = std::abs((int)b.y - (int)a.y);
    return (std::max)(dx, dy);
}

static float StepLen(const Vector2& a, const Vector2& b)
{
    int dx = std::abs((int)b.x - (int)a.x);
    int dy = std::abs((int)b.y - (int)a.y);

    return (dx != 0 && dy != 0) ? (1.41421356237f) : 1.0f;
}

std::vector<Vector2> EnemyUnit::CollectPathPoints() const
{
    std::vector<Vector2> pts;
    pts.reserve(8);


    Vector3 pos = _GetOwner()->GetTransform()->GetPosition();

    Vector2 p0;
    p0.x = std::round(pos.x / 2.0f);
    p0.y = std::round(pos.z / 2.0f);

    pts.push_back(p0); // 자신의 위치가 시작위치

    for (int i = 1; i < 8; ++i) // 0번이 시작위치인데, 시작위치 값을 안넣어줘서 1번부터 시작.
    {
        Vector2 p = GetPathPoint(i);
        if (p == GRIDPOS_INVALID) break;
        if (!pts.empty() && pts.back() == p) continue;
        pts.push_back(p);
    }
    return pts;
}


std::vector<ArrowSegment> EnemyUnit::GetArrowSegments() const
{
    std::vector<ArrowSegment> segs;

    std::vector<Vector2> pts = CollectPathPoints();
    if (pts.size() < 2) return segs;

    pts.front() = StepTowardGrid(pts[0], pts[1]);
    pts.back() = StepTowardGrid(pts.back(), pts[pts.size() - 2]);

    segs.reserve(pts.size() - 1);

    for (size_t i = 0; i + 1 < pts.size(); ++i)
    {
        Vector2 A = pts[i];
        Vector2 B = pts[i + 1];
        if (A == B) continue; // 두 지점이 같으면 그릴 필요가 없음. 

        float yaw = GetDirFromTo(A, B);
        int steps = GridDist(A, B);

        float stepLen = StepLen(A, B);
        float len = (std::max)(0, steps - 1) * stepLen;

        Vector3 wA = GridToWorld_A(A);
        Vector3 wB = GridToWorld_A(B);

        Vector3 mid = (wA + wB) / 2.0f;

        Vector3 dir = wB - wA;
        dir.y = 0.0f;

        float d = std::sqrt(dir.x * dir.x + dir.z * dir.z);
        if (d > 1e-6f) { dir.x /= d; dir.z /= d; }
        else { dir = Vector3{ 1,0,0 }; }

        Vector3 startPos = mid - dir * (len + 0.25f);
        Vector3 headPos = mid + dir * (len + 0.5f);

        segs.push_back(ArrowSegment{ headPos, startPos, mid, len, yaw });
    }

    return segs;
}
