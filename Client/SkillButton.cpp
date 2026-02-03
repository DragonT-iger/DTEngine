#include "SkillButton.h"
#include "GameObject.h"
#include "RayCastHitEvent.h"
#include "UIButton.h"
#include "GameManager.h"

BEGINPROPERTY(SkillButton)
DTPROPERTY(SkillButton, m_rayObj)
DTPROPERTY(SkillButton, m_attackButton)
DTPROPERTY(SkillButton, m_healButton)
ENDPROPERTY()


void SkillButton::Start()
{
		if (!m_rayObj || !m_attackButton || !m_healButton)
				return;

		// window 닫고 timescale 0으로.
		m_attackButton->SetOnClick([this]()
				{
						if (!m_isAttackUnlocked)
								return;

						auto rayEvent = m_rayObj->GetComponent<RayCastHitEvent>();
						if (!rayEvent)
								return;

						// 이미 스킬 활성화 중이면 무시
						if (rayEvent->GetAttackSkill() || rayEvent->GetHealSkill())
								return;

						rayEvent->CloseAllWindows();
						GameManager::Instance()->SetTimeScale(0);
						rayEvent->SetAttackSkill(true);
				});

		m_healButton->SetOnClick([this]()
				{
						if (!m_isHealUnlocked)
								return;

						auto rayEvent = m_rayObj->GetComponent<RayCastHitEvent>();
						if (!rayEvent)
								return;

						// 이미 스킬 활성화 중이면 무시
						if (rayEvent->GetAttackSkill() || rayEvent->GetHealSkill())
								return;

						rayEvent->CloseAllWindows();
						GameManager::Instance()->SetTimeScale(0.0f);
						rayEvent->SetHealSkill(true);
				});
}