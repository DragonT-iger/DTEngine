#include "pch.h"
#include "Canvas.h"
#include <algorithm>

BEGINPROPERTY(Canvas)
DTPROPERTY_ACCESSOR(Canvas, m_referenceResolution, GetReferenceResolution, SetReferenceResolution)
DTPROPERTY_ACCESSOR(Canvas, m_matchWidthOrHeight, GetMatchWidthOrHeight, SetMatchWidthOrHeight)
ENDPROPERTY()

float Canvas::GetScaleFactor(float screenWidth, float screenHeight) const
{
    if (m_referenceResolution.x <= 0.0f || m_referenceResolution.y <= 0.0f)
    {
        return 1.0f;
    }

    float scaleX = screenWidth / m_referenceResolution.x;
    float scaleY = screenHeight / m_referenceResolution.y;
    float match = std::clamp(m_matchWidthOrHeight, 0.0f, 1.0f);

    return scaleX * (1.0f - match) + scaleY * match;
}

Vector2 Canvas::GetCanvasSize(float screenWidth, float screenHeight) const
{
    if (m_referenceResolution.x > 0.0f && m_referenceResolution.y > 0.0f)
    {
        return m_referenceResolution;
    }

    return Vector2(screenWidth, screenHeight);
}
