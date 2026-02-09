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
						// raycasthit에 따로 함수로 빼둬서 저기서 알아서 toggle이랑 timescale 처리함.
						auto rayobj = m_rayobj->GetComponent<RayCastHitEvent>();
						rayobj->ApplySettingWindow();
				});
}
