#include "SecretTrigger.h"
#include "UIButton.h"
#include "GameObject.h"
#include "Levitating.h"

BEGINPROPERTY(SecretTrigger)
DTPROPERTY(SecretTrigger, m_obj)
DTPROPERTY(SecretTrigger, m_hiddenObj)
DTPROPERTY(SecretTrigger, m_button)
ENDPROPERTY()

void SecretTrigger::Start()
{
		if (!m_obj || !m_hiddenObj)
				return;

		// 색상 처리 없애기.
		m_button->SetHoverColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		m_button->SetPressedColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		
		m_button->SetOnClick([this]() 
				{
						m_hiddenObj->SetActive(true);

						auto btn = m_hiddenObj->GetComponent<UIButton>();
						if (btn)
						{
								btn->SetActive(true);
								btn->SetHoverColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
								btn->SetPressedColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
						}
						
						m_obj->SetActive(false);
						m_button->SetActive(false);
				});
}
