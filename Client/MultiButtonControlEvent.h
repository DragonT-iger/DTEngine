#pragma once
#include "MonoBehaviour.h"

class UIButton;
class ESCEvent;

class MultiButtonControlEvent : public MonoBehaviour
{
    DTGENERATED_BODY(MultiButtonControlEvent);

public:
    void Start() override;

private:
    void SetButtonsInteractable(bool interactable);

    UIButton* m_deactivateTriggerBtn = nullptr; 
    UIButton* m_activateTriggerBtn = nullptr;   

    UIButton* m_targetBtn1 = nullptr;
    UIButton* m_targetBtn2 = nullptr;
    UIButton* m_targetBtn3 = nullptr;
    UIButton* m_targetBtn4 = nullptr;

    ESCEvent* m_escEvent = nullptr;
};