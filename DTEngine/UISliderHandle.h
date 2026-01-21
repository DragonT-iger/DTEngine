#pragma once
#include "SimpleMathHelper.h"
#include "MonoBehaviour.h"

class UISliderHandle : public MonoBehaviour
{
    DTGENERATED_BODY(UISliderHandle);

public:
    UISliderHandle() = default;
    ~UISliderHandle() override = default;

    void Awake() override;
    void Update(float deltaTime) override;

    void SetParentSlider(class UISlider* slider) { m_parentSlider = slider; }

    // 마우스 이벤트 처리 
    void OnPointerDown(const Vector2& mousePos);
    void OnPointerDrag(const Vector2& mousePos);
    void OnPointerUp(const Vector2& mousePos);

    bool IsMouseOver(const Vector2& mousePos);
private:
    class UISlider* m_parentSlider = nullptr;
    class Transform* m_transform = nullptr;
    bool m_isDragging = false;
    float m_dragOffset = 0.0f;

    Vector2 m_dragStartPos = Vector2(0, 0);      // 드래그 시작 시 마우스 위치
    Vector3 m_handleStartPos = Vector3(0, 0, 0); // 드래그 시작 시 핸들 위치
};