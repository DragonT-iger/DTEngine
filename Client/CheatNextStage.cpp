#include "CheatNextStage.h"
#include "InputManager.h"
#include "GameManager.h"
#include "GameObject.h"
#include "ClickNextStageEvent.h"
#include "UIButton.h"

BEGINPROPERTY(CheatNextStage)
DTPROPERTY(CheatNextStage, m_button)
ENDPROPERTY()

void CheatNextStage::Update(float deltaTime)
{
    // InputManager 싱글톤 인스턴스 가져오기

    bool isPageUpHeld = InputManager::Instance().GetKey(KeyCode::Delete);
    bool isPageDownDown = InputManager::Instance().GetKeyDown(KeyCode::Home);

    bool isPageDownHeld = InputManager::Instance().GetKey(KeyCode::Home);
    bool isPageUpDown = InputManager::Instance().GetKeyDown(KeyCode::Delete);

    if (isPageUpHeld && isPageDownDown)
    {
        m_button->InvokeClick();
    }
}