#pragma once
#include "Unit.h"

// 적군

class EnemyUnit : public Unit
{
    DTGENERATED_BODY(EnemyUnit);
public:
    EnemyUnit() { SetTeam(Team::Enemy); }
    ~EnemyUnit() = default;

    const bool& IsBoss() const { return m_isBoss; }
    void SetBoss(bool isBoss);

    // 경로용
    Vector2 GetPathPoint(int i) const;
    int GetPathIndex() const { return m_pathIndex; }
    void SetPathIndex(int i) { m_pathIndex = (std::max)(0, (std::min)(i, 7)); }

private:
    bool m_isBoss = false;      // 보스라면
    float m_BossScale = 1.1f;   // 스텟 뻥튀기 
    
    int m_pathIndex = 0; // 현재 목표 인덱스.

    // 경로 등록용.
    Vector2 path0 = GRIDPOS_INVALID; // 시작점
    Vector2 path1 = GRIDPOS_INVALID;
    Vector2 path2 = GRIDPOS_INVALID;
    Vector2 path3 = GRIDPOS_INVALID;
    Vector2 path4 = GRIDPOS_INVALID;
    Vector2 path5 = GRIDPOS_INVALID;
    Vector2 path6 = GRIDPOS_INVALID;
    Vector2 path7 = GRIDPOS_INVALID;

};
