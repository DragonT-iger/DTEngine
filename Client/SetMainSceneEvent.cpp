#include "SetMainSceneEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "ClientSceneManager.h"

BEGINPROPERTY(SetMainSceneEvent)
DTPROPERTY(SetMainSceneEvent, m_button)
DTPROPERTY(SetMainSceneEvent, m_ObjWindow)
ENDPROPERTY()

void SetMainSceneEvent::Start()
{
		if (!m_button || !m_ObjWindow) return;

		auto OnClick = [this]()
		{
				if (m_ObjWindow->IsActive())
				{
					m_ObjWindow->SetActive(false);
				}
				ClientSceneManager::Instance().LoadScene("TitleScene");

		};
		m_button->SetOnClick(OnClick);
}
