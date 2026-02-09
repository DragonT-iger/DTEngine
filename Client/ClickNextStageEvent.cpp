#include "ClickNextStageEvent.h"  
#include "UIButton.h"
#include "GameObject.h"
#include "GameManager.h"

BEGINPROPERTY(ClickNextStageEvent)
DTPROPERTY(ClickNextStageEvent, m_button)
DTPROPERTY(ClickNextStageEvent, m_window)
ENDPROPERTY()


void ClickNextStageEvent::Start()
{
		if (!m_button || !m_window)
				return;

		m_button->SetOnClick([this]()
				{
						m_window->SetActive(false);

						GameManager::Instance()->LoadNextStageFromCurrent();
				});
}
