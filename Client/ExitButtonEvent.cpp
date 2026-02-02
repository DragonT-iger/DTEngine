#include "ExitButtonEvent.h"
#include "UIButton.h"
#include "GameObject.h"

BEGINPROPERTY(ExitButtonEvent)
DTPROPERTY(ExitButtonEvent, m_exitButton)
ENDPROPERTY()

// 부모 타고 가서 setactive false 해주기.
void ExitButtonEvent::Start()
{
		if (!m_exitButton)
				return;

		m_exitButton->SetOnClick([this]() 
				{
						_GetOwner()->GetTransform()->GetParent()->_GetOwner()->SetActive(false);
				});
}
