#pragma once
#include "MonoBehaviour.h"

class UIButton;

class RuleImageVisualEvent : public MonoBehaviour
{
    DTGENERATED_BODY(RuleImageVisualEvent);
public:
    void Start() override;

    void UpdateAllVisuals();      // 전체 초기화용
    void UpdateMoveVisuals();     // 이동 버튼만 갱신
    void UpdateBattleVisuals();   // 전투 버튼만 갱신

private:

    // 이동용
    UIButton* m_movefBtns;
    UIButton* m_movesBtns;
    UIButton* m_movetBtns;

    // 공격용
    UIButton* m_battlefBtns;
    UIButton* m_battlesBtns;
    UIButton* m_battletBtns;
};