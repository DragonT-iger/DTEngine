#pragma once
#include "MonoBehaviour.h"

class UIButton;
class EndSceneManager;

class EndSceneTrigger : public MonoBehaviour
{
    DTGENERATED_BODY(EndSceneTrigger);

public:
    void Start() override;

private:
    EndSceneManager* m_EndSceneManager = nullptr;

    UIButton* m_Button = nullptr;
};