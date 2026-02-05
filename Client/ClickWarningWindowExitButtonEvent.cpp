#include "ClickWarningWindowExitButtonEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "ClickStartButton.h"
#include "RayCastHitEvent.h"

BEGINPROPERTY(ClickWarningWindowExitButtonEvent)
DTPROPERTY(ClickWarningWindowExitButtonEvent, m_rayObj)
DTPROPERTY(ClickWarningWindowExitButtonEvent, m_startButton)
ENDPROPERTY()
void ClickWarningWindowExitButtonEvent::Start()
{
		if (!m_rayObj || !m_startButton)
				return;

		_GetOwner()->GetComponent<UIButton>()->SetOnClick([this]()
				{
						// 일단 부모 꺼주기.
						_GetOwner()->GetTransform()->GetParent()->_GetOwner()->SetActive(false);

						// ray 다시 처리되게 해주기.
						m_rayObj->GetComponent<RayCastHitEvent>()->SetRay(true);

						// startbutton 다시 눌릴 수 있게 처리.
						m_startButton->GetComponent<ClickStartButton>()->SetIsStart(false);
				});
}
