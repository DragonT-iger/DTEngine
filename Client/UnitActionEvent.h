#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class UnitActionEvent : public MonoBehaviour
{
    DTGENERATED_BODY(UnitActionEvent);
public: 
    UnitActionEvent() = default;
    virtual ~UnitActionEvent() = default;

    void UnitMove(Vector3 pos);
};
