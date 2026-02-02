#include "SkillButton.h"
#include "GameObject.h"
#include "RayCastHitEvent.h"
#include "UIButton.h"

BEGINPROPERTY(SkillButton)
DTPROPERTY(SkillButton, m_rayObj)
DTPROPERTY(SkillButton, m_attackButton)
DTPROPERTY(SkillButton, m_healButton)
ENDPROPERTY()


void SkillButton::Start()
{
		if (!m_rayObj || !m_attackButton || !m_healButton)
				return;

		// 있는 경우 각각 넣어주기.

		m_attackButton->SetOnClick([this]()
				{
						if (!m_isAttackUnlocked)
								return;

						m_rayObj->GetComponent<RayCastHitEvent>()->SetAttackSkill(true);
				});

		m_healButton->SetOnClick([this]()
				{
						if (!m_isHealUnlocked)
								return;

						m_rayObj->GetComponent<RayCastHitEvent>()->SetHealSkill(true);
				});
}