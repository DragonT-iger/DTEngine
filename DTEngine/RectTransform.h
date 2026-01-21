#pragma once

#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class RectTransform : public MonoBehaviour
{
    DTGENERATED_BODY(RectTransform);

public:
    RectTransform() = default;
    ~RectTransform() override = default;

    void SetAnchorMin(const Vector2& value);
    void SetAnchorMax(const Vector2& value);
    void SetPivot(const Vector2& value);
    void SetSizeDelta(const Vector2& value);
    void SetAnchoredPosition(const Vector2& value);
    void SetUseTransformAsSource(bool value) { m_useTransformAsSource = value; }

    const Vector2& GetAnchorMin() const { return m_anchorMin; }
    const Vector2& GetAnchorMax() const { return m_anchorMax; }
    const Vector2& GetPivot() const { return m_pivot; }
    const Vector2& GetSizeDelta() const { return m_sizeDelta; }
    const Vector2& GetAnchoredPosition() const { return m_anchoredPosition; }
    const bool& GetUseTransformAsSource() const { return m_useTransformAsSource; }

    Vector2 GetSize() const { return m_cachedSize; }
    Vector2 GetWorldCenter2D() const { return m_cachedWorldCenter; }

    Vector2 GetScreenPosition(float screenWidth, float screenHeight) const;

    void ApplyLayout(float screenWidth, float screenHeight);
    void ApplyLayoutRecursive(float screenWidth, float screenHeight);

private:
    RectTransform* GetParentRect() const;

    Vector2 m_anchorMin = Vector2(0.5f, 0.5f);
    Vector2 m_anchorMax = Vector2(0.5f, 0.5f);
    Vector2 m_pivot = Vector2(0.5f, 0.5f);
    Vector2 m_sizeDelta = Vector2(100.0f, 100.0f);
    Vector2 m_anchoredPosition = Vector2(0.0f, 0.0f);
    bool m_useTransformAsSource = false;

    Vector2 m_cachedSize = Vector2(0.0f, 0.0f);
    Vector2 m_cachedWorldCenter = Vector2(0.0f, 0.0f);
};
