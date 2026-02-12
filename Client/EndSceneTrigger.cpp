#include "EndSceneTrigger.h"
#include "EndSceneManager.h"
#include "UIButton.h"
#include "GameObject.h"

BEGINPROPERTY(EndSceneTrigger)
DTPROPERTY(EndSceneTrigger, m_EndSceneManager);
DTPROPERTY(EndSceneTrigger, m_Button);
ENDPROPERTY()

void EndSceneTrigger::Start()
{
    if (m_Button && m_EndSceneManager)
    {
        auto onClickFunc = [this]()
            {
                if (this->m_EndSceneManager)
                {
                    this->m_EndSceneManager->NextStep();
                }
            };

        m_Button->SetOnClick(onClickFunc);
    }
}