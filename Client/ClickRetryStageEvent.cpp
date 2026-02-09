#include "ClickRetryStageEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "GameManager.h"

BEGINPROPERTY(ClickRetryStageEvent)
DTPROPERTY(ClickRetryStageEvent, m_button)
DTPROPERTY(ClickRetryStageEvent, m_window)
ENDPROPERTY()

void ClickRetryStageEvent::Start()
{
		if (!m_button || !m_window)
				return;

		m_button->SetOnClick([this]()
				{
						// 패배 window 꺼주기.
						m_window->SetActive(false);

						GameManager::Instance()->ReloadCurrentStage();
				});
}

