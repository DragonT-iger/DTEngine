#include "pch.h"
#include "UISlider.h"
#include <algorithm>
#include <cmath>
#include "GameObject.h"
#include "Transform.h"
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
    // handle 자식 오브젝트로 강제로 생성시키고 등록을 awake에서 처리하자. 그러면 GameObject* handle로 가지고있고 
    m_Transform = GetComponent<Transform>();
    m_trackImage = GetComponent<Image>();

    CacheHandle();
    ApplyTrackColor();
    ApplyHandleColor();
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
    InvokeValueChanged();
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
            m_handleTransform = child;
            m_handleImage = child->_GetOwner()->GetComponent<Image>();
            break;
        }
    }

    if (m_handleImage)
    {
        ApplyHandleColor();
    }
}


void UISlider::InvokeValueChanged()
{
    if (!m_interactable) return;
    if (m_onValueChanged) m_onValueChanged(m_value);
}

void UISlider::Update(float deltaTime)
{
    if (!m_handleTransform)
    {
        return;
    }
  
    float range = m_maxValue - m_minValue;
    if (range <= 0.0f) return;

    Vector3 trackScale = m_Transform->GetScale();
    Vector3 handleScale = m_handleTransform->GetScale();
    Vector2 trackSize = Vector2(trackScale.x, trackScale.y);
    Vector2 handleSize = Vector2(handleScale.x, handleScale.y);
    float available = std::max(0.0f, trackSize.x - handleSize.x);

    float t = (m_value - m_minValue) / range;
    float x = -available * 0.5f + available * t;

    Vector3 localPos = m_handleTransform->GetPosition();
    localPos.x = x;
    localPos.y = 0.0f;
    m_handleTransform->SetPosition(localPos);
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
