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

    //const std::vector<Vector2>& GetPath() const { return m_path; }
    //void SetPath(const std::vector<Vector2>& path) { m_path = path; m_pathIndex = 0; }

private:
    bool m_isBoss = false;      // 보스라면
    float m_BossScale = 2.0f;   // 스텟 뻥튀기 

    //std::vector<Vector2> m_path;
    //int m_pathIndex = 0;


};
