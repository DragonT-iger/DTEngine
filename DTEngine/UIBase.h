#pragma once
#include "MonoBehaviour.h"

class UIBase : public MonoBehaviour
{
public:
    virtual ~UIBase() = default;

    virtual void RenderUI() = 0;

    void SetOrderInLayer(int order) { m_orderInLayer = order; }
    int GetOrderInLayer() const { return m_orderInLayer; }

protected:

    int m_orderInLayer = 0;
};