#include "pch.h"
#include "TutorialTrigger.h"
#include "UIButton.h"
#include "TutorialManager.h" 

BEGINPROPERTY(TutorialTrigger)
DTPROPERTY(TutorialTrigger, m_TutorialManager);
DTPROPERTY(TutorialTrigger, m_Button);
ENDPROPERTY()

void TutorialTrigger::Awake()
{
    if (m_Button && m_TutorialManager)
    {
        auto onClickFunc = [this]()
            {
                if (this->m_TutorialManager)
                {
                    this->m_TutorialManager->NextStep();
                }
            };

        m_Button->SetOnClick(onClickFunc);
    }
}