#pragma once
#include "MonoBehaviour.h"

class UIButton;
class TutorialManager;

class TutorialTrigger : public MonoBehaviour
{
    DTGENERATED_BODY(TutorialTrigger);

public:
    void Start() override;
	void Update(float deltaTime) override;

private:
    TutorialManager* m_TutorialManager = nullptr;

    UIButton* m_Button = nullptr;
};