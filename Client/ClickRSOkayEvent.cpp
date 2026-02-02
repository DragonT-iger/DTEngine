#include "ClickRSOkayEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "OpenRSEvent.h"
#include "AllyUnit.h"

BEGINPROPERTY(ClickRSOkayEvent)
DTPROPERTY(ClickRSOkayEvent, m_okayBtn)
ENDPROPERTY()



void ClickRSOkayEvent::Start()
{
    if (!m_okayBtn) return;

    m_okayBtn->SetOnClick([this]() 
        {
        // 부모(BG)로부터 이벤트 컴포넌트 획득
        auto bgEvent = _GetOwner()->GetTransform()->GetParent()->_GetOwner()->GetComponent<OpenRSEvent>();

        int mIdx = bgEvent->GetMoveIndex();
        int aIdx = bgEvent->GetBattleIndex();

        // 둘 다 0 이랑 같거나 3 미만일 때만 적용
        if (mIdx >= 0 && mIdx < 3 && aIdx >= 0 && aIdx < 3)
        {
            auto unit = bgEvent->GetTargetUnit();
            if (unit) 
            {
                auto alloy = unit->GetComponent<AllyUnit>();
                alloy->SetMoveRule(mIdx);
                alloy->SetBattleRule(aIdx);
            }
        }

        // 결과와 상관없이 창 닫기
        _GetOwner()->GetTransform()->GetParent()->_GetOwner()->SetActive(false);});
}
