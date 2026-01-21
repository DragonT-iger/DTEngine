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

    bool IsMouseOver(const Vector2& mousePos);
private:
    class UISlider* m_parentSlider = nullptr;
    class Transform* m_transform = nullptr;
    bool m_isDragging = false;

    Vector2 m_dragStartPos = Vector2(0, 0);      // 드래그 시작 시 마우스 위치
    Vector3 m_handleStartPos = Vector3(0, 0, 0); // 드래그 시작 시 핸들 위치
};