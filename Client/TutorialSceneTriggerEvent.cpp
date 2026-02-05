#include "pch.h"
#include "TutorialSceneTriggerEvent.h"
#include "UIButton.h"
#include "ClientSceneManager.h"


BEGINPROPERTY(TutorialSceneTriggerevent)
DTPROPERTY(TutorialSceneTriggerevent, m_Button);
ENDPROPERTY()


void TutorialSceneTriggerevent::Start()
{
	if (!m_Button) return;

	auto OnClick = [this]() {

		 ClientSceneManager::Instance().LoadScene("TutorialScene");
		};

	m_Button->SetOnClick(OnClick);

}
