#include "ClickWarningWindowOkayEvent.h"
#include "GameObject.h"
#include "ClickStartButton.h"
#include "UIButton.h"

BEGINPROPERTY(ClickWarningWindowOkayEvent)
DTPROPERTY(ClickWarningWindowOkayEvent, m_window)
DTPROPERTY(ClickWarningWindowOkayEvent, m_startButton)
ENDPROPERTY()

void ClickWarningWindowOkayEvent::Start()
{
		if (!m_window || !m_startButton)
				return;

		_GetOwner()->GetComponent<UIButton>()->SetOnClick([this]()
				{
						// window 끄고 게임 시작.
						m_window->SetActive(false);
						m_startButton->GetComponent<ClickStartButton>()->StartGame();
				});
}
