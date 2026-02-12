#include "SecretTrigger.h"
#include "UIButton.h"
#include "GameObject.h"

BEGINPROPERTY(SecretTrigger)
DTPROPERTY(SecretTrigger, m_obj)
DTPROPERTY(SecretTrigger, m_hiddenObj)
DTPROPERTY(SecretTrigger, m_button)
DTPROPERTY(SecretTrigger, m_hiddenButton)
ENDPROPERTY()

void SecretTrigger::Start()
{
		if (!m_obj || !m_hiddenObj || !m_button || !m_hiddenButton)
				return;
		// 4개 중 한개라도 없는게 아니고 m_hiddenobj가 그러면 가지고 있는거고 .
		// 색상 처리 없애기.
		m_button->SetHoverColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		m_button->SetPressedColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		
		m_hiddenButton->SetHoverColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		m_hiddenButton->SetPressedColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

		m_button->SetOnClick([this]() 
				{
						m_obj->SetActive(false);
						m_button->SetActive(false);
						m_hiddenObj->SetActive(true);
						m_hiddenButton->SetActive(true);
				});

		m_hiddenButton->SetOnClick([this]()
				{
						m_obj->SetActive(true);
						m_button->SetActive(true);
						m_hiddenObj->SetActive(false);
						m_hiddenButton->SetActive(false);
				});

		m_hiddenObj->SetActive(false);
		m_hiddenButton->SetActive(false);
}