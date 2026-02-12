#include "BossDialogueTrigger.h"
#include "DialogueController.h"
#include "UIButton.h"
#include "GameObject.h"

BEGINPROPERTY(BossDialogueTrigger)
DTPROPERTY(BossDialogueTrigger, m_dialogueController);
DTPROPERTY(BossDialogueTrigger, m_Button);
ENDPROPERTY()

void BossDialogueTrigger::Start()
{
    if (m_Button && m_dialogueController)
    {
        auto onClickFunc = [this]()
            {
                if (m_dialogueController)
                {
                    m_dialogueController->NextDialogue();
                }
            };

        m_Button->SetOnClick(onClickFunc);
    }
}