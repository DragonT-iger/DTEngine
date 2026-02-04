#include "ClickStartButton.h"
#include "UIButton.h"
#include "AllyUnit.h"
#include "GameObject.h"
#include "CombatController.h"
#include "RayCastHitEvent.h"
#include "Effect.h"

BEGINPROPERTY(ClickStartButton)
DTPROPERTY(ClickStartButton, m_startButton)
DTPROPERTY(ClickStartButton, m_combatObj)
DTPROPERTY(ClickStartButton, m_rayObj)
DTPROPERTY(ClickStartButton, m_tileMapObj)
ENDPROPERTY()


void ClickStartButton::Start()
{
		if (!m_startButton || !m_combatObj || !m_rayObj || !m_tileMapObj)
				return;

		m_startButton->SetOnClick([this]()
				{

						// 전투 시작 시 타일들 전부 깜빡거리는거 꺼주기 위해서.
						for (auto obj : m_tileMapObj->GetTransform()->GetChildren())
						{
								if (obj->GetName() == "Glow_W_Height_01_Red_Test" || obj->GetName() == "Glow_W_Height_01_White_Test")
								{
										auto effect = obj->GetComponent<Effect>();
										if (effect)
										{
												effect->SetAnimating(false);
												effect->SetEdgeColor(Vector4(0, 0, 0, 1));
										}
								}
						}

						m_combatObj->GetComponent<CombatController>()->Setup();
						m_rayObj->GetComponent<RayCastHitEvent>()->SetStartBattle(true);
				});
}


