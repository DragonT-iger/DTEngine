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
    class UIButton* m_button = nullptr;
    bool m_isDragging = false;
};