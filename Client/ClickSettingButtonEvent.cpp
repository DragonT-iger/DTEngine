#include "ClickSettingButtonEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "RayCastHitEvent.h"

BEGINPROPERTY(ClickSettingButtonEvent)
DTPROPERTY(ClickSettingButtonEvent, m_button)
DTPROPERTY(ClickSettingButtonEvent, m_rayobj)
ENDPROPERTY()

void ClickSettingButtonEvent::Start()
{
		if (!m_button || !m_rayobj)
				return;

		// window 토글 할거임.
		m_button->SetOnClick([this]()
				{		
						auto rayobj = m_rayobj->GetComponent<RayCastHitEvent>();
						rayobj->ToggleSettingWindow();
				});
}
