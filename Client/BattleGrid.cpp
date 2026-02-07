#include "BattleGrid.h"
#include "TilemapGenerator.h"
#include "TilemapData.h"     
#include "GameObject.h"

#include <queue>

BEGINPROPERTY(BattleGrid)
DTPROPERTY_SETTER(BattleGrid, m_tilemapGenerator, SetTilemapGenerator)
ENDPROPERTY()

void BattleGrid::SetTilemapGenerator(TilemapGenerator* tg)
{
    m_tilemapGenerator = tg;


    if (m_tilemapGenerator == nullptr) return;

    TilemapData* td = tg->GetMapData();


    int width = td->GetWidth();
    int height = td->GetHeight();

    Initialize(width, height);

    for (int h = 1; h < height + 1; ++h)
    {
        for (int w = 1; w < width + 1; ++w)
        {
            int tileIdx = td->GetTileIndex(w, h);

            if (tileIdx == 0 || tileIdx == 1 || tileIdx == 2 || tileIdx == 3) SetTile(Vector2{ (float)(w - 1), (float)(h - 1) });
            else if (tileIdx == 4) SetSolidWall(Vector2{ (float)(w - 1) , (float)(h - 1) });
            else if (tileIdx == 5) SetBreakableWall(Vector2{ (float)(w - 1), (float)(h - 1) });
            else if (tileIdx == 6) SetDefenseTile(Vector2{ (float)(w - 1), (float)(h - 1) }); // 인덱스 약속은 나중에 하면 될듯.
            else if (tileIdx == 7) SetTrapTile(Vector2{ (float)(w - 1), (float)(h - 1) });
            else { SetSolidWall(Vector2{ (float)(w - 1), (float)(h - 1) }); }
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
    t.trapTile = false;
}

void BattleGrid::SetSolidWall(const Vector2& p)
{
    auto& t = GetStaticTile(p);
    t.tile = true;
    t.solidWall = true;
    t.breakableWall = false;
    t.defenseTile = false;
    t.trapTile = false;
}

void BattleGrid::SetBreakableWall(const Vector2& p)
{
    auto& t = GetStaticTile(p);
    t.tile = true;
    t.solidWall = false;
    t.breakableWall = true; 
    t.defenseTile = false;
    t.trapTile = false;
}

void BattleGrid::SetDefenseTile(const Vector2& p)
{
    auto& t = GetStaticTile(p);
    t.tile = true;
    t.solidWall = false;
    t.breakableWall = false;
    t.defenseTile = true;
    t.trapTile = false;
}

void BattleGrid::SetTrapTile(const Vector2& p)
{
    auto& t = GetStaticTile(p);
    t.tile = true;
    t.solidWall = false;
    t.breakableWall = false;
    t.defenseTile = false;
    t.trapTile = true;
}

void BattleGrid::WallBreak(const Vector2& p)
{
    if (!InBounds(p)) return;
    GetStaticTile(p).breakableWall = false;
}

bool BattleGrid::IsBreakableWall(const Vector2& p) const
{
    if (!InBounds(p)) return false;
    return GetStaticTile(p).breakableWall;
}

bool BattleGrid::IsDefenseTile(const Vector2& p) const
{
    if (!InBounds(p)) return false;
    return GetStaticTile(p).defenseTile;
}

bool BattleGrid::IsTrapTile(const Vector2& p) const
{
    if (!InBounds(p)) return false;
    return GetStaticTile(p).trapTile;
}

bool BattleGrid::GetNearestDefenseTile(const Vector2& me, const Vector2& myMovePos, Vector2& outPos) const
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

            if (x == (int)me.x && y == (int)me.y) 
            {
                outPos = me;
                return true; 
            }

            if (x == (int)myMovePos.x && y == (int)myMovePos.y)
            {
                outPos = myMovePos;
                return true;
            }

            const BattleDynamicTile& d = m_dynamicGrid[i];
            if (d.unitPresent || d.reservedMove) continue;

            int dx = std::abs(x - (int)me.x);
            int dy = std::abs(y - (int)me.y);
            int dist = (std::max)(dx, dy);

            if (!found || dist < bestDist)
            {
                bestDist = dist;
                outPos = Vector2{ (float)x, (float)y };
                found = true;
            }
        }
    }

    return found;
}

void BattleGrid::ClearDynamicGrid()
{
    std::fill(m_dynamicGrid.begin(), m_dynamicGrid.end(), BattleDynamicTile{});
}

void BattleGrid::SyncUnitsPos(const std::vector<AllyUnit*>& allyUnits, 
    const std::vector<EnemyUnit*>& enemyUnits, const AliceUnit* aliceUnit, const RedQueenUnit* redQueenUnit)
{
    for (auto& d : m_dynamicGrid)
    {
        d.unitPresent = false;
    }

    for (const AllyUnit* ally : allyUnits)
    {
        if (!ally || !ally->IsAlive()) continue;
        const Vector2& p = ally->GetPos();
        GetDynamicTile(p).unitPresent = true;
    }

    for (const EnemyUnit* enemy : enemyUnits)
    {
        if (!enemy || !enemy->IsAlive()) continue;
        const Vector2& p = enemy->GetPos();
        GetDynamicTile(p).unitPresent = true;
    }

    if (aliceUnit)
    {
        const Vector2& p = aliceUnit->GetPos();
        GetDynamicTile(p).unitPresent = true;
    }

    if (redQueenUnit)
    {
        const Vector2& p = redQueenUnit->GetPos();
        GetDynamicTile(p).unitPresent = true;
    }
}

void BattleGrid::ReserveMove(const Vector2& p)
{
    if (!InBounds(p)) return;
    GetDynamicTile(p).reservedMove = true;
}

bool BattleGrid::IsReserved(const Vector2& p) const
{
    if (!InBounds(p)) return true;
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
    if (d.reservedMove)
    {
        if (me && p == me->GetMovePos()) return false; 
        return true;
    }

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

static int HeuristicOctile(const Vector2& a, const Vector2& b)
{
    int dx = std::abs((int)b.x - (int)a.x);
    int dy = std::abs((int)b.y - (int)a.y);
    int dmin = (std::min)(dx, dy);
    int dmax = (std::max)(dx, dy);
    return 14 * dmin + 10 * (dmax - dmin);
}

bool BattleGrid::FindNextStepAStar(const Vector2& start, const Vector2& goal,
    const Unit* me, const Unit* target, int moveRule, Vector2& outNext) const
{
    outNext = start;

    if (start == GRIDPOS_INVALID || goal == GRIDPOS_INVALID) return false;
    if (!InBounds(start) || !InBounds(goal)) return false;

    if (start == goal) // 목표칸이 시작점이면 시작점 반환.
    {
        outNext = start;
        return true;
    }

    if (IsBlocked(start, me, target, moveRule)) return false; // 시작칸이 막혀있으면 실패.

    const int N = m_width * m_height;

    std::vector<int> gScore(N, 999999);
    std::vector<int> parent(N, -1);
    std::vector<bool> closed(N, false);

    auto idxOf = [&](const Vector2& p) { return Index(p); };
    auto posOf = [&](int idx) -> Vector2 {
        int x = idx % m_width;
        int y = idx / m_width;
        return Vector2{ (float)x, (float)y };
        };

    struct Node { int f; int g; int idx; };
    struct Cmp {
        bool operator()(const Node& a, const Node& b) const {
            if (a.f != b.f) return a.f > b.f;      // f 작은 게 우선
            if (a.g != b.g) return a.g < b.g;      // 예: g 큰 걸 우선(탐색이 더 진행된 쪽)
            return a.idx > b.idx;                  // 완전 동점이면 인덱스로 고정
        }
    };

    int startIdx = idxOf(start);
    int goalIdx = idxOf(goal);

    gScore[startIdx] = 0;

    std::priority_queue<Node, std::vector<Node>, Cmp> open;
    open.push(Node{ HeuristicOctile(start, goal), 0, startIdx });

    const int dir8[8][3] = {
        { 0,-1,10}, { 1, 0,10}, { 0, 1,10}, {-1, 0,10},
        { 1,-1,14}, { 1, 1,14}, {-1, 1,14}, {-1,-1,14},
    };

    const bool allowCornerCut = false; // 코너 끼고 대각 이동 방지.

    while (!open.empty())
    {
        Node cur = open.top();
        open.pop();

        if (closed[cur.idx]) continue;
        if (cur.g != gScore[cur.idx]) continue;

        if (cur.idx == goalIdx)
        {
            int t = goalIdx;
            int prev = parent[t];

            while (prev != -1 && prev != startIdx)
            {
                t = prev;
                prev = parent[t];
            }
            outNext = posOf(t); // 시작칸의 다음칸
            return true;
        }

        closed[cur.idx] = true;
        Vector2 curPos = posOf(cur.idx);

        for (int k = 0; k < 8; ++k)
        {
            int sx = dir8[k][0];
            int sy = dir8[k][1];
            int stepCost = dir8[k][2];

            Vector2 nb{ curPos.x + (float)sx, curPos.y + (float)sy };
            if (!InBounds(nb)) continue;

            int nbIdx = idxOf(nb);
            if (closed[nbIdx]) continue;

            if (IsBlocked(nb, me, target, moveRule)) continue;

            if (!allowCornerCut && sx != 0 && sy != 0)
            {
                Vector2 side1{ curPos.x + (float)sx, curPos.y };
                Vector2 side2{ curPos.x, curPos.y + (float)sy };
                if (IsBlocked(side1, me, target, moveRule) || IsBlocked(side2, me, target, moveRule))
                    continue;
            }

            int tentativeG = gScore[cur.idx] + stepCost;
            if (tentativeG < gScore[nbIdx])
            {
                gScore[nbIdx] = tentativeG;
                parent[nbIdx] = cur.idx;

                int f = tentativeG + HeuristicOctile(nb, goal);
                open.push(Node{ f, tentativeG, nbIdx });
            }
        }
    }

    return false;
}

bool BattleGrid::FindNextStepGreedy(const Vector2& start, const Vector2& goal, 
    const Unit* me, const Unit* target, int moveRule, Vector2& outNext) const
{
    outNext = start;

    if (start == GRIDPOS_INVALID || goal == GRIDPOS_INVALID) return false;
    if (!InBounds(start) || !InBounds(goal)) return false;
    if (IsBlocked(start, me, target, moveRule)) return false;

    if (start == goal)
    {
        outNext = start;
        return true;
    }

    const int curH = HeuristicOctile(start, goal);

    Vector2 best = start;

    // 우선순위 키
    int bestBand = 999;      // 0:감소, 1:동일, 2:증가
    int bestH = 999999;      // 같은 band 내에서는 goal에 더 가까운(h가 작은) 칸 우선
    int bestReserved = 1;    // 예약 안 된 칸 우선
    int bestTie = 999999;    // 마지막 동점깨기

    const int dirs[8][2] = {
        { 0,-1}, { 1, 0}, { 0, 1}, {-1, 0},
        { 1,-1}, { 1, 1}, {-1, 1}, {-1,-1},
    };

    const bool allowCornerCut = false;

    auto tieScore = [&](const Vector2& nb) -> int
        {
            // 동점 깨기용. 더 세밀한 근접도(아무거나 OK)
            int dx = std::abs((int)goal.x - (int)nb.x);
            int dy = std::abs((int)goal.y - (int)nb.y);
            return dx + dy; // 맨해튼
        };

    for (int k = 0; k < 8; ++k)
    {
        int sx = dirs[k][0];
        int sy = dirs[k][1];

        Vector2 nb{ start.x + (float)sx, start.y + (float)sy };
        if (!InBounds(nb)) continue;
        if (IsBlocked(nb, me, target, moveRule)) continue;

        // 코너컷 금지(대각 이동 시 옆칸 둘 중 하나라도 막히면 제외)
        if (!allowCornerCut && sx != 0 && sy != 0)
        {
            Vector2 side1{ start.x + (float)sx, start.y };
            Vector2 side2{ start.x, start.y + (float)sy };
            if (IsBlocked(side1, me, target, moveRule) ||
                IsBlocked(side2, me, target, moveRule))
                continue;
        }

        int h = HeuristicOctile(nb, goal);
        int delta = h - curH;

        const int maxWorsen = 0; // 0(증가금지), 10(직선1칸), 14(대각1칸), 20(직선2칸)
        if (delta > maxWorsen) continue; 

        int band = (delta < 0) ? 0 : (delta == 0 ? 1 : 2);

        int reserved = GetDynamicTile(nb).reservedMove ? 1 : 0;
        int ts = tieScore(nb);

        bool better = false;

        if (best == start) better = true;
        else
        {
            if (band < bestBand) better = true;
            else if (band == bestBand)
            {
                // 같은 band면 goal에 더 가까운 칸(h가 작은 칸)
                if (h < bestH) better = true;
                else if (h == bestH)
                {
                    // 예약 안 된 칸 우선
                    if (reserved < bestReserved) better = true;
                    else if (reserved == bestReserved)
                    {
                        // 마지막 동점깨기
                        if (ts < bestTie) better = true;
                    }
                }
            }
        }

        if (better)
        {
            best = nb;
            bestBand = band;
            bestH = h;
            bestReserved = reserved;
            bestTie = ts;
        }
    }

    if (best == start) return false;
    outNext = best;
    return true;
}

bool BattleGrid::FindNextStepHybrid(const Vector2& start, const Vector2& goal,
    const Unit* me, const Unit* target, int moveRule, Vector2& outNext) const
{
    outNext = start;

    Vector2 aNext = start;
    Vector2 gNext = start;

    bool hasA = FindNextStepAStar(start, goal, me, target, moveRule, aNext);
    bool hasG = FindNextStepGreedy(start, goal, me, target, moveRule, gNext);

    if (!hasA && !hasG) return false;
    if (hasA && !hasG) { outNext = aNext; return true; }
    if (!hasA && hasG) { outNext = gNext; return true; }

    // 둘 다 있을 때 비교
    int curH = HeuristicOctile(start, goal);
    int aH = HeuristicOctile(aNext, goal);
    int gH = HeuristicOctile(gNext, goal);

    int aGain = curH - aH; // 클수록 좋음
    int gGain = curH - gH;

    // 임계치: greedy가 A*보다 "직선 1칸" 이상 더 진전이면 greedy 채택
    // (원하면 moveRule/유닛 타입에 따라 다르게 줘도 됨)
    const int greedyWinThreshold = 10;

    if (gGain >= aGain + greedyWinThreshold)
    {
        outNext = gNext;
        return true;
    }

    outNext = aNext;
    return true;
}


std::vector<GameObject*>& BattleGrid::GetEnemyObjects()
{
    TilemapData* td = m_tilemapGenerator->GetMapData();
    return m_tilemapGenerator->GetSpawnedEnemys();
}

GameObject* BattleGrid::GetAliceObjects()
{
    TilemapData* td = m_tilemapGenerator->GetMapData();
    return m_tilemapGenerator->GetSpawnedAlice();
}

GameObject* BattleGrid::GetRedQueenObjects()
{
    TilemapData* td = m_tilemapGenerator->GetMapData();
    return m_tilemapGenerator->GetSpawnedRedQueen();
}
