#include "pch.h"
#include "UIButton.h"
#include "Image.h"
#include "Scene.h"
#include "SceneManager.h"
#include "DX11Renderer.h"
#include "InputManager.h"

#include "SoundManager.h"

BEGINPROPERTY(UIButton)
DTPROPERTY_ACCESSOR(UIButton, m_interactable, GetInteractable, SetInteractable)
DTPROPERTY_ACCESSOR(UIButton, m_normalColor, GetNormalColor, SetNormalColor)
DTPROPERTY_ACCESSOR(UIButton, m_hoverColor, GetHoverColor, SetHoverColor)
DTPROPERTY_ACCESSOR(UIButton, m_pressedColor, GetPressedColor, SetPressedColor)
ENDPROPERTY()


void UIButton::InvokeClick()
{
    if (!m_interactable) return;

    for (const auto& callback : m_onClickEvents)
    {
        if (callback)
        {
            callback();
        }
    }
}

void UIButton::Update(float deltaTime)
{
    if (!m_interactable) return;

    float screenW, screenH;
#ifdef _DEBUG
    MousePos gameRes = InputManager::Instance().GetGameResolution();
    screenW = (float)gameRes.x;
    screenH = (float)gameRes.y;
#else
    screenW = (float)DX11Renderer::Instance().GetWidth();
    screenH = (float)DX11Renderer::Instance().GetHeight();
#endif

    const float refW = DX11Renderer::Instance().GetRefWidth();
    const float refH = DX11Renderer::Instance().GetRefHeight();

    float targetAspect = refW / refH;
    float windowAspect = screenW / screenH;

    float currentScale = 1.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    if (windowAspect > targetAspect)
    {
        currentScale = screenH / refH;
        float actualWidth = refW * currentScale;
        offsetX = (screenW - actualWidth) * 0.5f;
        offsetY = 0.0f; 
    }
    else
    {
        currentScale = screenW / refW;
        float actualHeight = refH * currentScale;
        offsetX = 0.0f; 
        offsetY = (screenH - actualHeight) * 0.5f;
    }

    auto mousePos = InputManager::Instance().GetGameMousePosition();

    float uiMouseX = ((float)mousePos.x - offsetX) / currentScale;
    float uiMouseY = ((float)mousePos.y - offsetY) / currentScale;

    Transform* tf = GetTransform();
    Vector3 pos = tf->GetWorldPosition();
    Vector3 size = tf->GetWorldScale();

    bool isInside = (uiMouseX >= pos.x && uiMouseX <= pos.x + size.x &&
        uiMouseY >= pos.y && uiMouseY <= pos.y + size.y);

    if (isInside)
    {
        SetHovered(true);

        if (InputManager::Instance().GetKeyDown(KeyCode::MouseLeft))
        {
            

            if (_GetOwner()->GetName() == "attackskill")
            {
                SoundManager::Instance().PlayOneShot("SFX/Skill_Poison_Button");
            }
            else if (_GetOwner()->GetName() == "healskill")
            {
                SoundManager::Instance().PlayOneShot("SFX/Skill_Heal_Button");
            }
            else
            {
                SoundManager::Instance().PlayOneShot("SFX/UI_Click_ver.2");
            }

            SetPressed(true);
        }
        else if (InputManager::Instance().GetKeyUp(KeyCode::MouseLeft))
        {
            if (m_isPressed)
            {
                InvokeClick();
            }
            SetPressed(false);
        }
    }
    else
    {
        SetHovered(false);

        if (InputManager::Instance().GetKeyUp(KeyCode::MouseLeft) || InputManager::Instance().GetKeyDown(KeyCode::MouseLeft))
        {
            SetPressed(false);
        }
    }
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

	//std::cout << "UIButton Pressed State Changed: " << (m_isPressed ? "Pressed" : "Released") << std::endl;
    
    //std::cout << "버튼 가지고 있는 오브젝트 이름 : " << _GetOwner()->GetName() << std::endl;
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
