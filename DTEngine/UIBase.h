#pragma once
#include "MonoBehaviour.h"

class UIBase : public MonoBehaviour
{
public:
    virtual ~UIBase() = default;

    virtual void RenderUI() = 0;
};