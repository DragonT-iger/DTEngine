#include "ESCEvent.h"
#include "GameObject.h"
#include "GameManager.h"
#include "InputManager.h"

BEGINPROPERTY(ESCEvent)
DTPROPERTY(ESCEvent, m_settingWindow)
DTPROPERTY(ESCEvent, m_window)
DTPROPERTY(ESCEvent, m_isCreditOpen)
ENDPROPERTY()


void ESCEvent::Update(float deltaTime)
{
		if (InputManager::Instance().GetKeyDown(KeyCode::Escape))
		{
				Check();
		}
}

void ESCEvent::Check()
{
		// 승패 결과창 뜨면 처리 안함.
		//if (GameManager::Instance() && GameManager::Instance()->IsResultInteractionLocked())
		//{
		//		return;
		//}
		if (m_isCreditOpen) {
			return;
		}

		if (m_window)
		{
				// 있으면 무조건 닫기.
				m_window->SetActive(false);
		}

		if (!m_settingWindow)
				return;

		bool isActive = m_settingWindow->IsActive();

		// 이전 배속 가져오기.
		int timeScale = GameManager::Instance()->GetPrevTimeScale();

		// 토글.
		m_settingWindow->SetActive(!isActive);

		// 켜지면 0 아니면 원래 배속.
		if (!isActive)
		{
				GameManager::Instance()->SetTimeScale(0);
		}
		else
		{
				GameManager::Instance()->SetTimeScale(timeScale);
		}
}
