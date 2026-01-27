#include "pch.h"
#include "RuleSelectOkayEvenet.h"
#include "AllyUnit.h"
#include "RuleSelectWindow.h"
#include "UIButton.h"


BEGINPROPERTY(RuleOkayEvent)
DTPROPERTY(RuleOkayEvent, m_ownerWindow)
ENDPROPERTY()

void RuleOkayEvent::Start()
{
    auto btn = GetComponent<UIButton>();
    if (btn)
    {
        btn->SetOnClick([this]() {
            this->OnClickConfirm();
            });
    }
}

void RuleOkayEvent::OnClickConfirm()
{
    if (!m_ownerWindow) return;

    AllyUnit* unit = m_ownerWindow->GetTargetUnit();
    int mIdx = m_ownerWindow->GetMoveIndex();
    int aIdx = m_ownerWindow->GetAttackIndex();

    // unit이 있고 1~3값 이내인 경우만 rule 바꿔주기.
    if (unit && mIdx > 0 && aIdx > 0 && mIdx < 4 && aIdx < 4)
    {
        unit->SetMoveRule((MoveRule)mIdx);
        unit->SetBattleRule((BattleRule)aIdx);
    }

    // 확인 눌렀으니까 닫기. 
    m_ownerWindow->Close();
}
