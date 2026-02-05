#include "pch.h"
#include "TitleCreditTriggerEvent.h"
#include "UIButton.h"


BEGINPROPERTY(TitleCreditTriggerEvent)
DTPROPERTY(TitleCreditTriggerEvent, m_Button);
DTPROPERTY(TitleCreditTriggerEvent, m_Credit);
DTPROPERTY(TitleCreditTriggerEvent, m_Setting);

ENDPROPERTY()

void TitleCreditTriggerEvent::Start()
{
	if (!m_Credit || !m_Button ||!m_Setting) return;

	auto OnClick = [this]()
		{
			m_Credit->SetInteractable(false);
			//m_Credit->SetActive(true);
			
			bool is_Active = m_Credit->_GetOwner()->IsActive();

			bool Setting_is_Active = m_Setting->_GetOwner()->IsActive();
			if (Setting_is_Active || is_Active)  m_Setting->_GetOwner()->SetActive(false);

			//Toggle
			
			 m_Credit->_GetOwner()->SetActive(!is_Active);


			

		
		};

	m_Button->SetOnClick(OnClick);

}
