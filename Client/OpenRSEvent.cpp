#include "OpenRSEvent.h"
#include "GameObject.h"
#include "Image.h"
#include "AllyUnit.h"
#include "RuleImageVisualEvent.h"
#include "AssetDatabase.h"

BEGINPROPERTY(OpenRSEvent)
DTPROPERTY(OpenRSEvent, m_windowBar)
ENDPROPERTY()

void OpenRSEvent::OpenWindow()
{
    _GetOwner()->SetActive(true);
}

void OpenRSEvent::RequestOpenWindow(GameObject* target)
{
    if (!target || !m_windowBar) return;

    GameObject* unitRoot = target;
    if (target->GetName().find("_V1") == std::string::npos) // 이름에 _Test가 없다면 자식으로 간주
    {
        if (target->GetTransform()->GetParent())
            unitRoot = target->GetTransform()->GetParent()->_GetOwner();
    }
    
    m_targetUnit = unitRoot;
    std::string unitName = m_targetUnit->GetName();

    // 상단 바 이미지 업데이트
    Image* barImage = m_windowBar->GetComponent<Image>();

    // AssetDatabase로 직접 가져오기. gameobject 만들어서 하지말고.
    if (unitName == "Chess_Sword_Shield_V1")
    {
        barImage->SetTextureID(AssetDatabase::Instance().GetIDFromPath("Assets/Models/UI/Alice_UI/Strategy_Knight.png"));
        m_targetUnit->GetComponent<Unit>()->SetUnitType(1);
    }
    else if (unitName == "Chess_Spear_V1")
    {
        barImage->SetTextureID(AssetDatabase::Instance().GetIDFromPath("Assets/Models/UI/Alice_UI/Strategy_Rook.png"));
        m_targetUnit->GetComponent<Unit>()->SetUnitType(0);
    }
    else if (unitName == "Chess_Wand_V1")
    {
        barImage->SetTextureID(AssetDatabase::Instance().GetIDFromPath("Assets/Models/UI/Alice_UI/Strategy_Bishop.png"));
        m_targetUnit->GetComponent<Unit>()->SetUnitType(2);
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

