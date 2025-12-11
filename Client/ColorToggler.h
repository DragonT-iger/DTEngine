#pragma once
#include "MonoBehaviour.h"
#include "ReflectionMacros.h"
#include "SimpleMathHelper.h" 

class ColorToggler : public MonoBehaviour
{
    DTGENERATED_BODY(ColorToggler);

public:
    ColorToggler() = default;
    virtual ~ColorToggler() = default;

    void Update(float deltaTime) override;

private:
    Vector4 m_firstColor = { 0.0f, 0.0f, 0.0f, 255.0f };  
    Vector4 m_secondColor = { 44.f, 160.f, 48.0f, 255.0f }; 

    bool m_isFirstColor = true;
};