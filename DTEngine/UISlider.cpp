#include "pch.h"
#include "UISlider.h"
#include <algorithm>
#include <cmath>
#include "GameObject.h"
#include "Transform.h"
#include "Image.h"
#include "Scene.h"
#include "SceneManager.h"
#include "UISliderHandle.h"

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

    // handle 찾기. 없는 경우 생성해서 자식으로.
    CacheHandle();

    // 초기 color값 set
    ApplyTrackColor();
    ApplyHandleColor();

    m_value = std::clamp(m_value, m_minValue, m_maxValue);
    UpdateHandlePosition();
}

void UISlider::Update(float deltaTime)
{
#ifndef _DEBUG  // 에디터 아닐 때만 자동 업데이트
    UpdateHandlePosition();
#endif
    if (m_handleComponent)
    {
        m_handleComponent->Update(deltaTime);
    }
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

    bool createdHandle = false;

    for (Transform* child : tf->GetChildren())
    {
        if (!child) continue;
        // handle 찾기.
        if (child->_GetOwner()->GetName() == "Handle")
        {
            m_handleTransform = child;
            m_handleImage = child->_GetOwner()->GetComponent<Image>();
            m_handleComponent = child->_GetOwner()->GetComponent<UISliderHandle>();
            createdHandle = true;
            break;
        }
    }

    // 못찾은 경우 생성.
    //if (!createdHandle)
    //{
    //    Scene* scene = SceneManager::Instance().GetActiveScene();
    //    if (!scene) return;

    //    GameObject* handle = scene->CreateUIImage("Handle");
    //    if (!handle) return;

    //    m_handleTransform = handle->GetTransform();
    //    m_handleTransform->SetParent(tf);
    //    m_handleTransform->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
    //    m_handleTransform->SetScale(Vector3(0.1f, 1.0f, 1.0f));

    //    m_handleImage = handle->GetComponent<Image>();
    //    if (m_handleImage && m_trackImage)
    //    {
    //        m_handleImage->SetOrderInLayer(m_trackImage->GetOrderInLayer() + 1);
    //    }

    //    m_handleComponent = handle->AddComponent<UISliderHandle>();
    //    createdHandle = true;
    //}

    if (m_handleComponent)
    {
        m_handleComponent->SetParentSlider(this);
        ApplyHandleColor();
    }
}

void UISlider::UpdateHandlePosition()
{
    if (!m_handleTransform || !m_Transform)
        return;

    float range = m_maxValue - m_minValue;
    if (range <= 0.0f) return;

    Vector3 trackScale = m_Transform->GetScale();
    Vector3 handleScale = m_handleTransform->GetScale();
    float trackWidth = trackScale.x;
    float handleWidth = handleScale.x;
    float available = std::max(0.0f, trackWidth - handleWidth);

    float t = (m_value - m_minValue) / range;
    float x = -available * 0.5f + available * t;

    Vector3 localPos = m_handleTransform->GetPosition();
    localPos.x = x;
    localPos.y = 0.0f;
    m_handleTransform->SetPosition(localPos);
}


void UISlider::OnHandleDragged(float mouseLocalX)
{
    if (!m_interactable || !m_handleTransform || !m_Transform) return;

    Vector3 trackScale = m_Transform->GetScale();
    Vector3 handleScale = m_handleTransform->GetScale();
    float trackWidth = trackScale.x;
    float handleWidth = handleScale.x;
    float available = std::max(0.0f, trackWidth - handleWidth);

    // 드래그 가능한 범위로 제한
    float minX = -available * 0.5f;
    float maxX = available * 0.5f;
    float clampedX = std::clamp(mouseLocalX, minX, maxX);

    // normalized position
    float normalizedPos = (clampedX - minX) / available;

    // value 계산 및 설정
    float newValue = m_minValue + (m_maxValue - m_minValue) * normalizedPos;
    SetValue(newValue);
}

void UISlider::InvokeValueChanged()
{
    if (!m_interactable) return;
    if (m_onValueChanged) m_onValueChanged(m_value);
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
