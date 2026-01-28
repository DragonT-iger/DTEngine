#include "OpenRSEvent.h"
#include "GameObject.h"
#include "Image.h"
#include "AllyUnit.h"

BEGINPROPERTY(OpenRSEvent)
DTPROPERTY(OpenRSEvent, m_windowBar)
DTPROPERTY(OpenRSEvent, m_imageFirst)
DTPROPERTY(OpenRSEvent, m_imageSecond)
DTPROPERTY(OpenRSEvent, m_imageThird)
ENDPROPERTY()


void OpenRSEvent::SetTargetUnit(GameObject* unit)
{
    std::cout << "settargetUnit 호출" << std::endl;
    m_targetUnit = unit;
    if (!m_targetUnit) return;

    if (!m_windowBar) return;

    // 유닛 이름에 따라 상단 바 이미지 변경
    Image* barImage = m_windowBar->GetComponent<Image>();
    std::string unitName = m_targetUnit->GetName();

    if (!m_imageFirst || !m_imageSecond || !m_imageThird) return;

    //if (unitName == "Knight") barImage->SetTexture(m_imageFirst->GetComponent<Image>()->GetTexture());
    //else if (unitName == "Rook") barImage->SetTexture(m_imageSecond->GetComponent<Image>()->GetTexture());
    //else if (unitName == "Bishop") barImage->SetTexture(m_imageThird->GetComponent<Image>()->GetTexture());

    if (unitName == "Dwarf") barImage->SetTexture(m_imageFirst->GetComponent<Image>()->GetTexture());
    else if (unitName == "MasterChief_v3") barImage->SetTexture(m_imageSecond->GetComponent<Image>()->GetTexture());
    else if (unitName == "Rabbit_MK_1") barImage->SetTexture(m_imageThird->GetComponent<Image>()->GetTexture());

    // BG 켜주기.
    OpenWindow();

    // 기존 유닛의 룰 값으로 인덱스 초기화. 없는 경우 visual update 안하고.
    auto alloy = m_targetUnit->GetComponent<AllyUnit>();
    if (alloy) 
    {
        m_moveIndex = alloy->GetMoveRule();
        m_battleIndex = alloy->GetBattleRule();
    }
}

void OpenRSEvent::OpenWindow()
{
    _GetOwner()->SetActive(true);
}
