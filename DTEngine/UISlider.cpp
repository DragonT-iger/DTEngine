#include "pch.h"
#include "UISlider.h"
#include <algorithm>
#include <cmath>
#include "GameObject.h"
#include "Transform.h"
#include "RectTransform.h"
#include "Image.h"

BEGINPROPERTY(UISlider)
DTPROPERTY_ACCESSOR(UISlider, m_minValue, GetMinValue, SetMinValue)
DTPROPERTY_ACCESSOR(UISlider, m_maxValue, GetMaxValue, SetMaxValue)
DTPROPERTY_ACCESSOR(UISlider, m_value, GetValue, SetValue)
DTPROPERTY_ACCESSOR(UISlider, m_wholeNumbers, GetWholeNumbers, SetWholeNumbers)
DTPROPERTY_ACCESSOR(UISlider, m_interactable, GetInteractable, SetInteractable)
DTPROPERTY_ACCESSOR(UISlider, m_fillColor, GetFillColor, SetFillColor)
DTPROPERTY_ACCESSOR(UISlider, m_trackColor, GetTrackColor, SetTrackColor)
DTPROPERTY_ACCESSOR(UISlider, m_handleColor, GetHandleColor, SetHandleColor)
ENDPROPERTY()

void UISlider::Awake()
{
    m_rectTransform = GetComponent<RectTransform>();
    m_trackImage = GetComponent<Image>();
    CacheHandle();
    ApplyTrackColor();
    ApplyHandleColor();
    UpdateHandleVisual();
}

void UISlider::SetValue(float value)
{
    if (m_wholeNumbers)
    {
        value = std::round(value);
    }

    float clamped = std::clamp(value, m_minValue, m_maxValue);

    if (clamped == m_value) return;

    m_value = clamped;
    UpdateHandleVisual();
    InvokeValueChanged();
}

void UISlider::InvokeValueChanged()
{
    if (!m_interactable) return;
    if (m_onValueChanged) m_onValueChanged(m_value);
}

void UISlider::CacheHandle()
{
    Transform* tf = GetTransform();
    if (!tf) return;

    for (Transform* child : tf->GetChildren())
    {
        if (!child) continue;
        if (child->_GetOwner()->GetName() == "Handle")
        {
            m_handleRect = child->_GetOwner()->GetComponent<RectTransform>();
            m_handleImage = child->_GetOwner()->GetComponent<Image>();
            break;
        }
    }

    if (m_handleImage)
    {
        ApplyHandleColor();
    }
}

void UISlider::UpdateHandleVisual()
{
    if (!m_rectTransform || !m_handleRect) return;

    float range = m_maxValue - m_minValue;
    if (range <= 0.0f) return;

    Vector2 trackSize = m_rectTransform->GetSize();
    Vector2 handleSize = m_handleRect->GetSize();
    float available = std::max(0.0f, trackSize.x - handleSize.x);

    float t = (m_value - m_minValue) / range;
    float x = -available * 0.5f + available * t;

    m_handleRect->SetAnchoredPosition(Vector2(x, 0.0f));
}

void UISlider::ApplyTrackColor()
{
    if (m_trackImage)
    {
        m_trackImage->SetColor(m_trackColor);
    }
}

void UISlider::ApplyHandleColor()
{
    if (m_handleImage)
    {
        m_handleImage->SetColor(m_handleColor);
    }
}
