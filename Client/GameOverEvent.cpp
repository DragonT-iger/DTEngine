#include "GameOverEvent.h"
#include "UIButton.h"
#include "GameObject.h"

BEGINPROPERTY(GameOverEvent)
DTPROPERTY(GameOverEvent, m_button)
ENDPROPERTY()

void GameOverEvent::Start()
{
		if (!m_button)
				return;

		m_button->SetOnClick([this]()
				{
						// 여기서 이제 해야하는거는 클릭이 된 경우에 이 함수 호출되는거는 맞는거니까 
						// windows.h 를 가져와서 꺼버리는건 그렇잖아. 걍 어디선가 있을테니까 gamemanager에서 windows.h파일을 읽었으면 
						// 그냥 거기 include 해서 거기에 해당하는 gameover 함수가 있을거니까 그걸 호출할려고 하는데 gamemanager는 지금 거의 비어있으니까
						// 그래서 여기서 엄 어캐하지 라는 생각중..
						
						// dx11renderer를 읽어버리고 거기에서 함수호출 끝. 
				});
}
