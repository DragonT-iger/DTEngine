#include "BattleGrid.h"
#include "TilemapGenerator.h"
#include "TilemapData.h"     
#include "GameObject.h"

BEGINPROPERTY(BattleGrid)
DTPROPERTY_SETTER(BattleGrid, m_tilemapGenerator, SetTilemapGenerator)
ENDPROPERTY()

void BattleGrid::SetTilemapGenerator(TilemapGenerator* tg)
{
    m_tilemapGenerator = tg;
    TilemapData* td = tg->GetMapData();

    int width = td->GetWidth();
    int height = td->GetHeight();

    Initialize(width, height);

    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {
            int tileIdx = td->GetTileIndex(w, h);

            if (tileIdx == 0) SetTile(Vector2{ (float)w, (float)h });
            else if (tileIdx == 1) SetSolidWall(Vector2{ (float)w, (float)h });
            else if (tileIdx == 2) SetBreakableWall(Vector2{ (float)w, (float)h });
            else if (tileIdx == 3) SetDefenseTile(Vector2{ (float)w, (float)h }); // 인덱스 약속은 나중에 하면 될듯.
            else { /*뭔가 이상한 타일.. 암튼 필요없는 정보*/ }
        }
    }

    /*for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            const BattleStaticTile& t = m_staticGrid[y * m_width + x];

            if (t.tile)
            {
                if (t.solidWall)        std::cout << "벽 ";
                else if (t.breakableWall) std::cout << "부 ";
                else if (t.defenseTile) std::cout << "방 ";
                else                    std::cout << "타 ";
            }
            else
            {
                std::cout << "비 ";
            }
        }
        std::cout << '\n';
    }*/
}

void BattleGrid::Initialize(int width, int height)
{
    m_width = width;
    m_height = height;
    m_staticGrid.assign((size_t)width * height, BattleStaticTile{});
    m_dynamicGrid.assign((size_t)width * height, BattleDynamicTile{});
}

void BattleGrid::SetTile(const Vector2& p)
{
    auto& t = GetStaticTile(p);
    t.tile = true;
    t.solidWall = false;
    t.breakableWall = false;
    t.defenseTile = false;
}

void BattleGrid::SetSolidWall(const Vector2& p)
{
    auto& t = GetStaticTile(p);
    t.tile = true;
    t.solidWall = true;
    t.breakableWall = false;
    t.defenseTile = false;
}

void BattleGrid::SetBreakableWall(const Vector2& p)
{
    auto& t = GetStaticTile(p);
    t.tile = true;
    t.solidWall = false;
    t.breakableWall = true; 
    t.defenseTile = false;
}

void BattleGrid::SetDefenseTile(const Vector2& p)
{
    auto& t = GetStaticTile(p);
    t.tile = true;
    t.solidWall = false;
    t.breakableWall = false;
    t.defenseTile = true;
}

void BattleGrid::WallBreak(const Vector2& p)
{
    GetStaticTile(p).breakableWall = false;
}

bool BattleGrid::IsBreakableWall(const Vector2& p) const
{
    return GetStaticTile(p).breakableWall;
}

bool BattleGrid::GetNearestDefenseTile(const Vector2& me, Vector2& outPos) const
{
    bool found = false;
    int bestDist = 9999;

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            int i = y * m_width + x;

            const BattleStaticTile& s = m_staticGrid[i];
            if (!s.tile || !s.defenseTile) continue;

            const BattleDynamicTile& d = m_dynamicGrid[i];
            if (d.unitPresent || d.reservedMove) continue;

            int dx = std::abs(x - (int)me.x);
            int dy = std::abs(y - (int)me.y);
            int dist = dx + dy;

            if (!found || dist < bestDist)
            {
                bestDist = dist;
                outPos = Vector2{ (float)x, (float)y };
                found = true;

                if (bestDist == 0) return true; // 내가 이미 방어타일 위에 있으면 더 찾을 필요 없음. 
            }
        }
    }

    return found;
}

void BattleGrid::ClearDynamicGrid()
{
    std::fill(m_dynamicGrid.begin(), m_dynamicGrid.end(), BattleDynamicTile{});
}

void BattleGrid::SyncUnitsPos(const std::vector<AllyUnit*>& allyUnits, const std::vector<EnemyUnit*>& enemyUnits)
{
    for (auto& d : m_dynamicGrid)
    {
        d.unitPresent = false;
    }

    for (const AllyUnit* ally : allyUnits)
    {
        if (!ally) continue;
        const Vector2& p = ally->GetPos();
        GetDynamicTile(p).unitPresent = true;
    }

    for (const EnemyUnit* enemy : enemyUnits)
    {
        if (!enemy) continue;
        const Vector2& p = enemy->GetPos();
        GetDynamicTile(p).unitPresent = true;
    }
}

void BattleGrid::ReserveMove(const Vector2& p)
{
    GetDynamicTile(p).reservedMove = true;
}

bool BattleGrid::IsReserved(const Vector2& p) const
{
    return GetDynamicTile(p).reservedMove;
}

bool BattleGrid::IsBlocked(const Vector2& p, const Unit* me, const Unit* target, int moveRule) const
{
    if (!InBounds(p)) return true;

    const BattleStaticTile& s = GetStaticTile(p);
    const BattleDynamicTile& d = GetDynamicTile(p);

    // 타일 없음
    if (!s.tile) return true;

    // 벽
    if (s.solidWall) return true;

    // 부서지는 벽
    if (s.breakableWall) if (moveRule != MoveRule::Chase) return true; // 추격은 예외   


    // 유닛이 존재하는 타일
    if (d.unitPresent)
    {
        if (me && p == me->GetPos()) return false; // 본인의 위치라면 안막힌거로.

        if (target && p == target->GetPos()) return false; // 이동목표의 위치라면 안막힌거로.

        return true;
    }

    // 이동 예약
    if (d.reservedMove) return true;

    return false;
}


bool BattleGrid::IsInRange(const Vector2& me, const Vector2& target, int range) const
{
    int dx = std::abs((int)target.x - (int)me.x);
    int dy = std::abs((int)target.y - (int)me.y);

    return ((std::max)(dx, dy) <= range) && (dx != 0 || dy != 0); // 체비쇼프 거리로 비교 & 본인 위치 제외.
}

bool BattleGrid::IsAttackBlocked(const Vector2& p, const Vector2& me, const Vector2& target) const
{
    if (!InBounds(p)) return true;

    const auto& s = GetStaticTile(p);
    const auto& d = GetDynamicTile(p);

    //if (!s.tile) return true;
    if (s.solidWall) return true;
    if (s.breakableWall) return true; // 벽, 부벽 모두 공격을 막음

    if (d.unitPresent) // 다른 유닛도 공격을 막음
    {
        if (p == me) return false;
        if (p == target) return false; // 타겟 칸의 유닛은 맞아야하니까..
        return true;
    }

    return false;
}

bool BattleGrid::HasLineOfSight(const Vector2& me, const Vector2& target) const // 나랑 상대 사이 칸들 공격 막히는지 검사. 브레젠험.
{
    int x0 = (int)me.x;
    int y0 = (int)me.y;
    int x1 = (int)target.x;
    int y1 = (int)target.y;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (!(x0 == x1 && y0 == y1)) // 시작칸이랑 도착칸은 검사 안함. 
    {
        int e2 = err * 2;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }

        if (x0 == x1 && y0 == y1) break;

        Vector2 p{ (float)x0, (float)y0 };
        if (IsAttackBlocked(p, me, target)) return false;
    }

    return true;
}
