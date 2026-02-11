#include "MultiButtonControlEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "ESCEvent.h"

BEGINPROPERTY(MultiButtonControlEvent)
DTPROPERTY(MultiButtonControlEvent, m_escEvent);

DTPROPERTY(MultiButtonControlEvent, m_deactivateTriggerBtn)
DTPROPERTY(MultiButtonControlEvent, m_activateTriggerBtn)

DTPROPERTY(MultiButtonControlEvent, m_targetBtn1)
DTPROPERTY(MultiButtonControlEvent, m_targetBtn2)
DTPROPERTY(MultiButtonControlEvent, m_targetBtn3)
DTPROPERTY(MultiButtonControlEvent, m_targetBtn4)
ENDPROPERTY()

void MultiButtonControlEvent::Start()
{
    if (m_deactivateTriggerBtn)
    {
        m_deactivateTriggerBtn->SetOnClick([this]()
            {
                SetButtonsInteractable(false);
                m_escEvent->SetCreditOpen(true);
            });
    }

    if (m_activateTriggerBtn)
    {
        m_activateTriggerBtn->SetOnClick([this]()
            {
                SetButtonsInteractable(true);
                m_escEvent->SetCreditOpen(false);
            });
    }
}

void MultiButtonControlEvent::SetButtonsInteractable(bool interactable)
{
    if (m_targetBtn1) m_targetBtn1->SetInteractable(interactable);
    if (m_targetBtn2) m_targetBtn2->SetInteractable(interactable);
    if (m_targetBtn3) m_targetBtn3->SetInteractable(interactable);
    if (m_targetBtn4) m_targetBtn4->SetInteractable(interactable);
}