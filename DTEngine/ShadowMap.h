#pragma once
#include "MonoBehaviour.h"

class ShadowMap : public MonoBehaviour
{
    DTGENERATED_BODY(ShadowMap);

public:
    ShadowMap() = default;
    ~ShadowMap() = default;

    float m_size = 30.0f;       
};