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
#include "UIButton.h"

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
    UpdateHandlePosition();

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
    if (!createdHandle)
    {
        Scene* scene = SceneManager::Instance().GetActiveScene();
        if (!scene) return;

        GameObject* handle = scene->CreateGameObject("Handle");
        if (!handle) return;

        m_handleTransform = handle->GetTransform();
        m_handleTransform->SetParent(tf);
        m_handleTransform->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

        // y값은 동일한 size로. x는 10분의 1로.
        m_handleTransform->SetScale(Vector3(0.1f, 1.0f, 1.0f));

        m_handleImage = handle->GetComponent<Image>();
        m_handleComponent = handle->AddComponent<UISliderHandle>();
        createdHandle = true;
    }

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

    float minX = 0.0f;
    float maxX = 0.0f;
    float available = 0.0f;
    if (!ComputeHandleBounds(minX, maxX, available))
        return;

    float t = (m_value - m_minValue) / range;
    float x = minX + available * t;

    Vector3 localPos = m_handleTransform->GetPosition();
    localPos.x = x;
    localPos.y = 0.0f;
    m_handleTransform->SetPosition(localPos);
}


void UISlider::OnHandleDragged(float mouseLocalX)
{
    if (!m_interactable || !m_handleTransform || !m_Transform) return;

    float minX = 0.0f;
    float maxX = 0.0f;
    float available = 0.0f;
    if (!ComputeHandleBounds(minX, maxX, available))
        return;

    if (available <= 0.0f) return;
    Vector3 trackPos = m_Transform->GetPosition();
    Vector3 trackScale = m_Transform->GetScale();

    // 마우스를 track 기준 로컬 좌표로 변환 후 정규화
    float localMouseX = mouseLocalX - trackPos.x;
    float normalizedMouseX = localMouseX / trackScale.x;

    float clampedX = std::clamp(normalizedMouseX, minX, maxX);
    Vector3 localPos = m_handleTransform->GetPosition();
    localPos.x = clampedX;
    localPos.y = 0.0f;
    m_handleTransform->SetPosition(localPos);
}

void UISlider::OnHandleReleased(float mouseLocalX)
{
    if (!m_interactable || !m_handleTransform || !m_Transform) return;

    float minX = 0.0f;
    float maxX = 0.0f;
    float available = 0.0f;
    if (!ComputeHandleBounds(minX, maxX, available))
        return;

    if (available <= 0.0f)
    {
        SetValue(m_minValue);
        return;
    }

    Vector3 trackPos = m_Transform->GetPosition();
    Vector3 trackScale = m_Transform->GetScale();

    // 마우스를 track 기준 로컬 좌표로 변환 후 정규화
    float localMouseX = mouseLocalX - trackPos.x;
    float normalizedMouseX = localMouseX / trackScale.x;

    float clampedX = std::clamp(normalizedMouseX, minX, maxX);
    float normalizedPos = (clampedX - minX) / available;
    float newValue = m_minValue + (m_maxValue - m_minValue) * normalizedPos;
    SetValue(newValue);
    UpdateHandlePosition();
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

    if (m_handleTransform)
    {
        if (auto* button = m_handleTransform->_GetOwner()->GetComponent<UIButton>())
        {
            button->SetNormalColor(m_handleColor);
            button->SetHoverColor(m_handleColor);
            button->SetPressedColor(m_handleColor);
        }
    }
}

bool UISlider::ComputeHandleBounds(float& minX, float& maxX, float& available) const
{
    if (!m_Transform || !m_handleTransform)
        return false;

    //Vector3 trackScale = m_Transform->GetScale();
    //Vector3 handleScale = m_handleTransform->GetScale();
    //float trackWidth = trackScale.x;
    //float handleWidth = handleScale.x;
    //available = std::max(0.0f, trackWidth - handleWidth);
    //
    //// minX = -available * 0.5f;
    //// maxX = available * 0.5f;
    //// 왼쪽 끝을 기준으로. 이전에는 중앙 기준이였음.

    //minX = 0.0f;
    //maxX = available;

    // handle의 scale을 고려하여 이동 가능 범위 계산
    
    Vector3 handleScale = m_handleTransform->GetScale();
    float handleWidth = handleScale.x;

    // 0~1 범위에서 handle 크기만큼 빼서 제한.
    available = 1.0f - handleWidth;
    minX = 0.0f;
    maxX = 1.0f;
    return true;
}
