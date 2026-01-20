#include "pch.h"
#include "UIButton.h"
#include "Image.h"
#include "Scene.h"
#include "SceneManager.h"

BEGINPROPERTY(UIButton)
DTPROPERTY_ACCESSOR(UIButton, m_interactable, GetInteractable, SetInteractable)
DTPROPERTY_ACCESSOR(UIButton, m_normalColor, GetNormalColor, SetNormalColor)
DTPROPERTY_ACCESSOR(UIButton, m_hoverColor, GetHoverColor, SetHoverColor)
DTPROPERTY_ACCESSOR(UIButton, m_pressedColor, GetPressedColor, SetPressedColor)
ENDPROPERTY()

void UIButton::Awake()
{
    // 조건 충족 안되면 생성 안시킬거임.
    if (m_canvas)
    {
        return;
    }

    // scene 있는지 체크.
    Scene* scene = SceneManager::Instance().GetActiveScene();
    if (!scene)
    {
        return;
    }

    // canvas 있는 obj 찾기 및 등록.
    for (const auto& obj : scene->GetGameObjects())
    {
        if (!obj)
        {
            continue;
        }

        Canvas* canvas = obj->GetComponent<Canvas>();
        if (canvas)
        {
            m_canvas = canvas;
            break;
        }
    } 
    
    ApplyNormalState();

    // canvas 없으면 삭제.
    if (!m_canvas)
    {
        Destroy(_GetOwner());
    }
}

void UIButton::InvokeClick()
{
    if (!m_interactable) return;
    if (m_onClick) m_onClick();
}

void UIButton::ApplyNormalState()
{
    ApplyImageColor(m_normalColor);
}

void UIButton::ApplyHoverState()
{
    ApplyImageColor(m_hoverColor);
}

void UIButton::ApplyPressedState()
{
    ApplyImageColor(m_pressedColor);
}

void UIButton::SetHovered(bool hovered)
{
    if (m_isHovered == hovered)
    {
        return;
    }
    m_isHovered = hovered;
    UpdateVisualState();
}

void UIButton::SetPressed(bool pressed)
{
    if (m_isPressed == pressed)
    {
        return;
    }
    m_isPressed = pressed;
    UpdateVisualState();
}

void UIButton::ApplyImageColor(const Vector4& color)
{
    if (auto* image = GetComponent<Image>())
    {
        image->SetColor(color);
    }
}

void UIButton::UpdateVisualState()
{
    if (!m_interactable)
    {
        ApplyNormalState();
        return;
    }

    if (m_isPressed)
    {
        ApplyPressedState();
        return;
    }

    if (m_isHovered)
    {
        ApplyHoverState();
        return;
    }

    ApplyNormalState();
}
