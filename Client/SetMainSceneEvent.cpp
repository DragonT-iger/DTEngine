#include "SetMainSceneEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "SceneManager.h"

BEGINPROPERTY(SetMainSceneEvent)
DTPROPERTY(SetMainSceneEvent, m_button)
ENDPROPERTY()

void SetMainSceneEvent::Start()
{
		if (!m_button)
				return;

		// mainscene load해주기. 추후에 mainscene이 생기면 주석 풀기.
		/*m_button->SetOnClick([this]()
				{
						SceneManager::Instance().LoadScene("MainScene");
				});*/
}
