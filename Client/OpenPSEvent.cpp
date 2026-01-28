#include "OpenPSEvent.h"
#include "Gameobject.h"
#include "RayCastHitEvent.h"
#include "SelectIndexEvent.h"
#include "Transform.h"
#include "ImageVisualUpdateEvent.h"

BEGINPROPERTY(OpenPSEvent)
DTPROPERTY(OpenPSEvent, m_rayObject)
ENDPROPERTY()



void OpenPSEvent::SetActivePSWindow()
{
		// 이미 켜져 있다면 일단 다시 꺼주자.
		if (_GetOwner()->IsActive())
		{
				_GetOwner()->SetActive(false);
		}
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
