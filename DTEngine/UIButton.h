#pragma once

#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"
#include "Canvas.h"
#include <functional>

class UIButton : public MonoBehaviour
{
    DTGENERATED_BODY(UIButton);

public:
    UIButton() = default;
    ~UIButton() override = default;

    void Awake() override;

    void SetInteractable(bool value) { m_interactable = value; }
    const bool& GetInteractable() const { return m_interactable; }

    void SetNormalColor(const Vector4& value) { m_normalColor = value; }
    const Vector4& GetNormalColor() const { return m_normalColor; }

    void SetHoverColor(const Vector4& value) { m_hoverColor = value; }
    const Vector4& GetHoverColor() const { return m_hoverColor; }

    void SetPressedColor(const Vector4& value) { m_pressedColor = value; }
    const Vector4& GetPressedColor() const { return m_pressedColor; }

    void ApplyNormalState();
    void ApplyHoverState();
    void ApplyPressedState();

    void SetHovered(bool hovered);
    bool IsHovered() const { return m_isHovered; }

    void SetPressed(bool pressed);
    bool IsPressed() const { return m_isPressed; }

    void SetOnClick(std::function<void()> callback) { m_onClick = std::move(callback); }
    void InvokeClick();

private:
    void ApplyImageColor(const Vector4& color);
    void UpdateVisualState();

    // 컬러값 default.
    Vector4 m_normalColor = Vector4(1.f, 1.f, 1.f, 1.f);
    Vector4 m_hoverColor = Vector4(0.8f, 0.8f, 0.8f, 1.f);
    Vector4 m_pressedColor = Vector4(0.6f, 0.6f, 0.6f, 1.f);

    std::function<void()> m_onClick;    // 함수 저장용.

    Canvas* m_canvas = nullptr;

    bool m_interactable = true; // 상호작용
    bool m_isHovered = false;  
    bool m_isPressed = false;
};
