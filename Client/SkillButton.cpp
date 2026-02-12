#include "SkillButton.h"
#include "GameObject.h"
#include "RayCastHitEvent.h"
#include "UIButton.h"
#include "GameManager.h"
#include "Text.h"

BEGINPROPERTY(SkillButton)
DTPROPERTY(SkillButton, m_rayObj)
DTPROPERTY(SkillButton, m_attackButton)
DTPROPERTY(SkillButton, m_healButton)

DTPROPERTY(SkillButton, m_attackCountText)
DTPROPERTY(SkillButton, m_healCountText)

DTPROPERTY(SkillButton, m_skillGuideObj)
ENDPROPERTY()


void SkillButton::Start()
{
		if (!m_rayObj || !m_attackButton || !m_healButton || !m_attackCountText || !m_healCountText)
				return;

		int attackCount = GameManager::Instance()->GetAttackSkillcount();
		int healCount = GameManager::Instance()->GetHealSkillcount();

		m_attackCountText->SetText(std::to_wstring(attackCount));
		m_healCountText->SetText(std::to_wstring(healCount));


		// window 닫고 timescale 0으로.
		// 이제 GameManager Count로 체크.
		m_attackButton->SetOnClick([this]()
				{
						
						int attackCount = GameManager::Instance()->GetAttackSkillcount();
						std::cout << attackCount << std::endl;
						if (attackCount <= 0)
								return;

						auto rayEvent = m_rayObj->GetComponent<RayCastHitEvent>();
						if (!rayEvent)
								return;

						// 게임 시작 안했으면 처리 안함.
						bool start = rayEvent->GetStartBattle();
						if (!start)
								return;

						// 이미 스킬 활성화 중이면 무시
						if (rayEvent->GetAttackSkill() || rayEvent->GetHealSkill())
								return;

						if (m_skillGuideObj)
						{
								m_skillGuideObj->SetActive(true);
						}
								

						rayEvent->CloseAllWindows();
						GameManager::Instance()->SetTimeScale(0);
						rayEvent->SetAttackSkill(true);
				});

		m_healButton->SetOnClick([this]()
				{
						
						int healCount = GameManager::Instance()->GetHealSkillcount();
						std::cout << healCount << std::endl;

						if (healCount <= 0)
								return;

						auto rayEvent = m_rayObj->GetComponent<RayCastHitEvent>();
						if (!rayEvent)
								return;

						// 게임 시작 안했으면 처리 안함.
						bool start = rayEvent->GetStartBattle();
						if (!start)
								return;

						// 이미 스킬 활성화 중이면 무시
						if (rayEvent->GetAttackSkill() || rayEvent->GetHealSkill())
								return;

						if (m_skillGuideObj)
						{
								m_skillGuideObj->SetActive(true);
						}
			
						rayEvent->CloseAllWindows();
						GameManager::Instance()->SetTimeScale(0.0f);
						rayEvent->SetHealSkill(true);
				});
}