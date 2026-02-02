#pragma once
#include "MonoBehaviour.h"

class UIButton;

class ClickRSOkayEvent : public MonoBehaviour
{
    DTGENERATED_BODY(ClickRSOkayEvent);
public:
    void Start() override;
private:
    UIButton* m_okayBtn = nullptr;
};

