#include "SetActiveEvent.h"
#include "../DTEngine/UIButton.h"
#include "../DTEngine/GameObject.h"

BEGINPROPERTY(SetActiveEvent)
DTPROPERTY(SetActiveEvent, m_button)
ENDPROPERTY()

void SetActiveEvent::Start()
{
	m_button = this->GetComponent<UIButton>();
	if (m_button)
	{
		m_button->SetOnClick([this]() 
			{
				bool currentActive = IsActive();
				_GetOwner()->SetActive(!currentActive);
			});
	}
}
