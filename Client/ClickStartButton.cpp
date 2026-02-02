#include "ClickStartButton.h"
#include "UIButton.h"
#include "AllyUnit.h"
#include "GameObject.h"
#include "CombatController.h"
#include "RayCastHitEvent.h"

BEGINPROPERTY(ClickStartButton)
DTPROPERTY(ClickStartButton, m_startButton)
DTPROPERTY(ClickStartButton, m_combatObj)
DTPROPERTY(ClickStartButton, m_rayObj)
ENDPROPERTY()


void ClickStartButton::Start()
{
		if (!m_startButton || !m_combatObj || !m_rayObj)
				return;

		m_startButton->SetOnClick([this]()
				{
						m_combatObj->GetComponent<CombatController>()->Setup();
						m_rayObj->GetComponent<RayCastHitEvent>()->SetStartBattle(true);
				});
}

