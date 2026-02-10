#include "SkillHoverEvent.h"
#include "GameObject.h"
#include "UIButton.h"
#include "Image.h"

BEGINPROPERTY(SkillHoverEvent)
DTPROPERTY(SkillHoverEvent, m_skillButton)
DTPROPERTY(SkillHoverEvent, m_skillWindowObj)
ENDPROPERTY()

void SkillHoverEvent::Update(float deltaTime)
{
    // 안켜져있으면 처리 안할거임.
    auto img = _GetOwner()->GetComponent<Image>();

    if (!img->IsActive() || !img || !m_skillButton || !m_skillWindowObj)
        return;

    // hover 상태 가져와서 윈도우 상태랑 비교 후 다를 때 변경.
    bool shouldBeActive = m_skillButton->IsHovered();

    if (m_skillWindowObj->IsActive() != shouldBeActive)
    {
        m_skillWindowObj->SetActive(shouldBeActive);
    }
}
