#pragma once

#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class Canvas : public MonoBehaviour
{
    DTGENERATED_BODY(Canvas);

public:
    Canvas() = default;
    ~Canvas() override = default;

    void Awake() override;

    void SetReferenceResolution(const Vector2& value) { m_referenceResolution = value; }
    const Vector2& GetReferenceResolution() const { return m_referenceResolution; }

    void SetMatchWidthOrHeight(float value) { m_matchWidthOrHeight = value; }
    const float& GetMatchWidthOrHeight() const { return m_matchWidthOrHeight; }

    Vector2 GetCanvasSize(float screenWidth, float screenHeight) const;
    float GetScaleFactor(float screenWidth, float screenHeight) const;

private:
    Vector2 m_referenceResolution = Vector2(1280, 720);
    float m_matchWidthOrHeight = 0.0f;
};
