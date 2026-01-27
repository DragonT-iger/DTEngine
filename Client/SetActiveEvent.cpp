#include "SetActiveEvent.h"
#include "../DTEngine/UIButton.h"
#include "../DTEngine/GameObject.h"
#include "Scene.h"
#include "SceneManager.h"
BEGINPROPERTY(SetActiveEvent)
DTPROPERTY(SetActiveEvent, m_button)
ENDPROPERTY()

void SetActiveEvent::Start()
{
		Scene* scene = SceneManager::Instance().GetActiveScene();
		
		m_button = this->GetComponent<UIButton>();
	if (m_button)
	{
		m_button->SetOnClick([this]() 
			{
				bool currentActive = IsActive();
				_GetOwner()->SetActive(!currentActive);
			});
	}
}
