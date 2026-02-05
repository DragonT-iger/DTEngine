#include "MakeSetActiveEvent.h"
#include "../DTEngine/UIButton.h"
#include "../DTEngine/GameObject.h"

BEGINPROPERTY(MakeSetActive)
DTPROPERTY(MakeSetActive, m_button)
DTPROPERTY(MakeSetActive, m_toBeActivated_button)
DTPROPERTY(MakeSetActive, m_objWindow)

ENDPROPERTY()

void MakeSetActive::Start()
{
	if (!m_button || !m_toBeActivated_button || !m_objWindow) return;

	auto OnClick = [this]()
		{
			if (m_objWindow->IsActive())
			{
				m_objWindow->SetActive(false);
			}
			bool Is_Active = m_toBeActivated_button->_GetOwner()->IsActive();
			m_toBeActivated_button->_GetOwner()->SetActive(!Is_Active);
		};

	m_button->SetOnClick(OnClick);


}