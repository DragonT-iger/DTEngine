#include "OpenPSEvent.h"
#include "Gameobject.h"
#include "RayCastHitEvent.h"
#include "SelectIndexEvent.h"
#include "Transform.h"
#include "ImageVisualUpdateEvent.h"
#include "DelayDisableEvent.h"

BEGINPROPERTY(OpenPSEvent)
DTPROPERTY(OpenPSEvent, m_rayObject)
DTPROPERTY(OpenPSEvent, m_unitRuleWindow)
DTPROPERTY(OpenPSEvent, m_nonCostObj)
ENDPROPERTY()



void OpenPSEvent::SetActivePSWindow()
{
		// 이미 켜져 있다면 일단 다시 꺼주자.
		if (_GetOwner()->IsActive())
		{
				_GetOwner()->SetActive(false);
		}
		
		// 켜진 상태에서 끈 경우 timer 멈추고 다시 보이기 때문에 막아주기.
		if (m_nonCostObj)
		{
				m_nonCostObj->GetComponent<DelayDisableEvent>()->SetTimer(0);
				m_nonCostObj->SetActive(false);
		}

		if (!m_unitRuleWindow)
				return;
		
		// 도움말 끄기.
		m_unitRuleWindow->SetActive(false);

		// 화면 켜주기.
		_GetOwner()->SetActive(true);

		// 켜져있는 상태.
		m_isOpen = true;

		auto selectEvent = _GetOwner()->GetComponent<SelectIndexEvent>();
		if (selectEvent)
		{
				selectEvent->SetIndex(0);
		}

		auto visualUpdate = _GetOwner()->GetTransform()->GetChildren();
		for (auto value : visualUpdate)
		{
				if (value->GetComponent<ImageVisualUpdateEvent>())
				{
						value->GetComponent<ImageVisualUpdateEvent>()->UpdateAllButtonsVisual();
				}
		}
		
		m_rayobjTransform = m_rayObject->GetTransform();
}
