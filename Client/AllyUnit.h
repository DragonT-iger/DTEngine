#pragma once
#include "Unit.h"

// 아군

enum MoveRule
{
    Hold = 0, // 대기
    Advance,  // 전진
    Chase     // 추격
};

enum BattleRule
{
    Nearest = 0,     // 가장 가까운 적
    LowestHp,        // 체력 낮은 적
    TypeAdvantage,   // 상성 우위
};

class AllyUnit : public Unit 
{
    DTGENERATED_BODY(AllyUnit);

public:
    AllyUnit() { SetTeam(Team::Ally); }
    ~AllyUnit() = default;

    const int& GetMoveRule() const { return m_moveRule; }
    void SetMoveRule(int r) { m_moveRule = r; }

    const int& GetBattleRule() const { return m_battleRule; }
    void SetBattleRule(int r) { m_battleRule = r; }
     
private:
    int m_moveRule = MoveRule::Hold;
    int m_battleRule = BattleRule::Nearest;
};

