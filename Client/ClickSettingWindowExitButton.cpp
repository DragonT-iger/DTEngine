#include "ClickSettingWindowExitButton.h"
#include "GameObject.h"
#include "UIButton.h"
#include"RayCastHitEvent.h"

BEGINPROPERTY(ClickSettingWindowExitButton)
DTPROPERTY(ClickSettingWindowExitButton, m_rayObj)
ENDPROPERTY()

void ClickSettingWindowExitButton::Start()
{
		if (m_rayObj)
		{
				_GetOwner()->GetComponent<UIButton>()->SetOnClick([this]()
						{
								m_rayObj->GetComponent<RayCastHitEvent>()->ApplySettingWindow();
						});
		}
}
