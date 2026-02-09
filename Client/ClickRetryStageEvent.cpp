#include "ClickRetryStageEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "GameManager.h"
#include "ClientSceneManager.h"
#include "Scene.h"

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


						//GameManager::Instance()->NextStage();
						// life 줄이기.
						int life = GameManager::Instance()->GetLife();
						GameManager::Instance()->SetLife(life - 1);
						ClientSceneManager::Instance().LoadScene("MainGameScene");


						//GameManager::Instance()->ReloadCurrentStage();
				});
}

