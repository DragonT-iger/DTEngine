#include "ClickSetActiveObjEvent.h"
#include "UIButton.h"
#include "GameObject.h"

BEGINPROPERTY(ClickSetActiveObjEvent)
DTPROPERTY(ClickSetActiveObjEvent, m_button)
DTPROPERTY(ClickSetActiveObjEvent, m_obj)
DTPROPERTY(ClickSetActiveObjEvent, m_windowObj)
ENDPROPERTY()

void ClickSetActiveObjEvent::Start()
{
		if (!m_button || !m_obj)
				return;

		m_button->SetOnClick([this]()
				{
						// rule info 에서 button 두개 인 경우 다른거 추가해서 끄고 한개만 켜지게 하기.
						if (m_windowObj)
						{
								m_windowObj->SetActive(false);
						}
						// toggle | toggle 아니고 그냥 켜지는 버튼으로만.
						m_obj->SetActive(true);
				});
}
