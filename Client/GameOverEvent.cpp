#include "GameOverEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "DX11Renderer.h"

BEGINPROPERTY(GameOverEvent)
DTPROPERTY(GameOverEvent, m_button)
ENDPROPERTY()

void GameOverEvent::Start()
{
		if (!m_button)
				return;

		m_button->SetOnClick
		(
			[this]()
				{
				PostMessage(DX11Renderer::Instance().GetHandle(), WM_CLOSE, 0, 0);
			}
		);
}
