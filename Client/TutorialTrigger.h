#pragma once
#include "MonoBehaviour.h"

class UIButton;
class TutorialManager;

class TutorialTrigger : public MonoBehaviour
{
    DTGENERATED_BODY(TutorialTrigger);

public:
    virtual void Awake() override;

private:
    TutorialManager* m_TutorialManager = nullptr;

    UIButton* m_Button = nullptr;
};