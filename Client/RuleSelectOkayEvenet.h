#pragma once
#include "MonoBehaviour.h"

class RuleSelectWindow;

class RuleOkayEvent : public MonoBehaviour
{
    DTGENERATED_BODY(RuleOkayEvent);
public:
    void Start() override;

private:
    void OnClickConfirm();

    // 연결해줘야함.
    RuleSelectWindow* m_ownerWindow = nullptr;
};
