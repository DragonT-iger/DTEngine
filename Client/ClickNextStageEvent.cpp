#include "ClickNextStageEvent.h"  
#include "UIButton.h"
#include "GameObject.h"
#include "GameManager.h"
#include "ClientSceneManager.h"

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



						GameManager::Instance()->NextStage();
						ClientSceneManager::Instance().LoadScene("MainGameScene");
						// 이건 GameManager의 curStage 값을 올리고 그냥 그 씬을 재로드 해야겠지
						//GameManager::Instance()->LoadNextStageFromCurrent();
				});
}
