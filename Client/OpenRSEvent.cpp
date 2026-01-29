#include "OpenRSEvent.h"
#include "GameObject.h"
#include "Image.h"
#include "AllyUnit.h"
#include "RuleImageVisualEvent.h"

BEGINPROPERTY(OpenRSEvent)
DTPROPERTY(OpenRSEvent, m_windowBar)
DTPROPERTY(OpenRSEvent, m_imageFirst)
DTPROPERTY(OpenRSEvent, m_imageSecond)
DTPROPERTY(OpenRSEvent, m_imageThird)
ENDPROPERTY()


void OpenRSEvent::OpenWindow()
{
    _GetOwner()->SetActive(true);
}

void OpenRSEvent::RequestOpenWindow(GameObject* target)
{
    if (!target || !m_windowBar) return;

    GameObject* unitRoot = target;
    if (target->GetName().find("_Test") == std::string::npos) // 이름에 _Test가 없다면 자식으로 간주
    {
        if (target->GetTransform()->GetParent())
            unitRoot = target->GetTransform()->GetParent()->_GetOwner();
    }

    m_targetUnit = unitRoot;
    std::string unitName = m_targetUnit->GetName();

    // 상단 바 이미지 업데이트
    Image* barImage = m_windowBar->GetComponent<Image>();
    if (m_imageFirst && m_imageSecond && m_imageThird)
    {
        if (unitName == "Chess_Sword_Shield_Test") barImage->SetTexture(m_imageFirst->GetComponent<Image>()->GetTexture());
        else if (unitName == "Chess_Spear_Test") barImage->SetTexture(m_imageSecond->GetComponent<Image>()->GetTexture());
        else if (unitName == "Chess_Wand_Test") barImage->SetTexture(m_imageThird->GetComponent<Image>()->GetTexture());
    }

    // 이동, 전투 값 가져오기.
    auto alloy = m_targetUnit->GetComponent<AllyUnit>();
    if (alloy)
    {
        m_moveIndex = alloy->GetMoveRule();
        m_battleIndex = alloy->GetBattleRule();
    }

    auto visual = GetComponent<RuleImageVisualEvent>();
    if (visual)
    {
        visual->UpdateAllVisuals();
    }

    OpenWindow();
}

