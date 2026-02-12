#pragma once
#include "MonoBehaviour.h"

class UIButton;
class DialogueController;

class BossDialogueTrigger : public MonoBehaviour
{
    DTGENERATED_BODY(BossDialogueTrigger);

public:
    void Start() override;

private:
    DialogueController* m_dialogueController = nullptr;

    UIButton* m_Button = nullptr;
};