#include "ExitButtonEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "GameManager.h"

BEGINPROPERTY(ExitButtonEvent)
DTPROPERTY(ExitButtonEvent, m_exitButton)
DTPROPERTY(ExitButtonEvent, m_settingWindow)
ENDPROPERTY()

// 부모 타고 가서 setactive false 해주기.
void ExitButtonEvent::Start()
{
		if (!m_exitButton)
				return;

		m_exitButton->SetOnClick([this]() 
				{
						// 만약 settingwindow면 timescale 복구해주기.
						if (m_settingWindow)
						{
								int ts = GameManager::Instance()->GetPrevTimeScale();
								GameManager::Instance()->SetTimeScale(ts);
						}
						_GetOwner()->GetTransform()->GetParent()->_GetOwner()->SetActive(false);
				});
}
