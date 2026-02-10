#include "LoadSceneEvent.h"

#include "UIButton.h"
#include "ClientSceneManager.h"

BEGINPROPERTY(LoadSceneEvent)

DTPROPERTY(LoadSceneEvent, m_sceneName)

ENDPROPERTY()


void LoadSceneEvent::Start() {

	GetComponent<UIButton>()->SetOnClick([this]() {
		
		ClientSceneManager::Instance().LoadScene(m_sceneName);
			
		});
	
}