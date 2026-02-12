#pragma once
#include "MonoBehaviour.h"

class UIButton;

class CheatNextStage : public MonoBehaviour
{
    DTGENERATED_BODY(CheatNextStage);
public:
    void Update(float deltaTime) override;

private:
    UIButton* m_button = nullptr;
};