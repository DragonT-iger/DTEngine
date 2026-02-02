#pragma once
#include <vector>
#include <algorithm>

#include "MonoBehaviour.h"
#include "Unit.h"
#include "AllyUnit.h"
#include "EnemyUnit.h"
#include "AliceUnit.h"

class Unit;
class AllyUnit;
class EnemyUnit;
class AliceUnit;

class TilemapGenerator;
class TilemapData;

struct BattleStaticTile // 정적타일 정보  
{
    bool tile = false;          // 타일
    bool solidWall = false;     // 벽
    bool breakableWall = false; // 부서지는 벽
    bool defenseTile = false;   // 방어타일
    bool trapTile = false; // 함정타일
};
// 타일만 true면 그냥 타일, 타일 + 벽이 true면 벽, 타일 + 부벽이 true면 부벽..  

struct BattleDynamicTile // 동적타일 정보 // 매번 지우고 갱신해줘야하는 정보.
{
    bool unitPresent = false;   // 현재 유닛 점유
    bool reservedMove = false;  // 이동 예정 위치
};

class BattleGrid : public MonoBehaviour
{
    DTGENERATED_BODY(BattleGrid);
public:
    BattleGrid() = default;
    ~BattleGrid() = default;

    void SetTilemapGenerator(TilemapGenerator* tg);

    void Initialize(int width, int height); // 타일 정보 저장하기.
    bool InBounds(const Vector2& p) const { return (p.x >= 0 && p.y >= 0 && p.x < m_width && p.y < m_height); }
    int GetWidth() { return m_width; }
    int GetHeight() { return m_height; }


    void SetTile(const Vector2& p);
    void SetSolidWall(const Vector2& p);
    void SetBreakableWall(const Vector2& p);
    void SetDefenseTile(const Vector2& p);
    void SetTrapTile(const Vector2& p);

    int Index(const Vector2& p) const {
        int x = static_cast<int>(p.x);
        int y = static_cast<int>(p.y);
        return y * m_width + x;
    }

    BattleStaticTile& GetStaticTile(const Vector2& p) { return m_staticGrid[Index(p)]; }
    const BattleStaticTile& GetStaticTile(const Vector2& p) const { return m_staticGrid[Index(p)]; }

    BattleDynamicTile& GetDynamicTile(const Vector2& p) { return m_dynamicGrid[Index(p)]; }
    const BattleDynamicTile& GetDynamicTile(const Vector2& p) const { return m_dynamicGrid[Index(p)]; }


    // 이동로직에 필요한 함수들.
    void WallBreak(const Vector2& p); // 벽 파괴! 
    bool IsBreakableWall(const Vector2& p) const; // 이 위치가 부벽인지.
    bool IsDefenseTile(const Vector2& p) const; // 이 위치가 방어타일인지.
    bool IsTrapTile(const Vector2& p) const; // 이 위치가 함정타일인지.

    bool GetNearestDefenseTile(const Vector2& me, const Vector2& myMovePos, Vector2& outPos) const; // 내 위치로부터 가까운 방어타일 위치 얻기. 

    void ClearDynamicGrid(); // 유닛 관련 위치 정보 초기화.
    
    void SyncUnitsPos(const std::vector<AllyUnit*>& allyUnits, 
        const std::vector<EnemyUnit*>& enemyUnits, const AliceUnit* aliceUnit); // 유닛 위치 동기화.

    void ReserveMove(const Vector2& p); // 이동목표지점 선점.
    bool IsReserved(const Vector2& p) const; // 여기가 누군가 이미 선점한 이동목표지점인지.

    bool IsBlocked(const Vector2& p, const Unit* me, const Unit* target, int moveRule) const; // 여기가 장애물인지.

    bool IsInRange(const Vector2& me, const Vector2& target, int range) const; // 공격/인식범위 안인지.
    bool IsAttackBlocked(const Vector2& p, const Vector2& me, const Vector2& target) const; // 공격이 막히는지.
    bool HasLineOfSight(const Vector2& me, const Vector2& target) const; // LoS 검사. 공격 경로 중간에 벽, 부벽, 유닛이 끼어 있나?

    bool FindNextStepAStar(const Vector2& start, const Vector2& goal,
        const Unit* me, const Unit* target, int moveRule, Vector2& outNext) const;

private:
    int m_width = 0;
    int m_height = 0;

    std::vector<BattleStaticTile>  m_staticGrid;
    std::vector<BattleDynamicTile> m_dynamicGrid;

    TilemapGenerator* m_tilemapGenerator;
};