#pragma once
#include "SimpleMathHelper.h"
#include "MonoBehaviour.h"
#include <functional>
#include "UIBase.h"

class UIButton;

class UISlider : public MonoBehaviour
{
    DTGENERATED_BODY(UISlider);

public:
    UISlider() = default;
    ~UISlider() override = default;

    void Awake() override;
    void Update(float deltaTime) override;

    void SetMinValue(float value) { m_minValue = value; }
    const float& GetMinValue() const { return m_minValue; }

    void SetMaxValue(float value) { m_maxValue = value; }
    const float& GetMaxValue() const { return m_maxValue; }

    void SetValue(float value);
    const float& GetValue() const { return m_value; }

    // 정수값만 사용할려면 체크.
    void SetWholeNumbers(bool value) { m_wholeNumbers = value; }
    const bool& GetWholeNumbers() const { return m_wholeNumbers; }
    
    // 상호작용 안할거면 체크. 체크하면 callback 함수 호출 안함.
    void SetInteractable(bool value) { m_interactable = value; }
    const bool& GetInteractable() const { return m_interactable; }

    void SetFillColor(const Vector4& value) { m_fillColor = value; }
    const Vector4& GetFillColor() const { return m_fillColor; }

    void SetTrackColor(const Vector4& value) { m_trackColor = value; }
    const Vector4& GetTrackColor() const { return m_trackColor; }

    void SetHandleColor(const Vector4& value) { m_handleColor = value; }
    const Vector4& GetHandleColor() const { return m_handleColor; }

    // callback 함수 저장.
    void SetOnValueChanged(std::function<void()> callback) { m_onValueChanged = callback; }
    
    // callback 함수 호출.
    void InvokeValueChanged();

    // handle 찾아서 저장.
    void CacheHandle();

    void OnHandleDragged(float mouseLocalX);
    void OnHandleReleased(float mouseLocalX);

    void UpdateHandlePosition();
private:
    void ApplyTrackColor();
    void ApplyHandleColor();
    bool ComputeHandleBounds(float& minX, float& maxX, float& available) const;

    class Transform* m_Transform = nullptr;
    class Transform* m_handleTransform = nullptr;
    class Image* m_handleImage = nullptr;
    class Image* m_trackImage = nullptr;
    class UISliderHandle* m_handleComponent = nullptr;

    float m_minValue = 0.0f;
    float m_maxValue = 1.0f;
    float m_value = 0.0f;
    bool m_wholeNumbers = false;
    bool m_interactable = true;
    Vector4 m_fillColor = Vector4(1.f, 1.f, 1.f, 1.f);
    Vector4 m_trackColor = Vector4(1.f, 1.f, 1.f, 1.f);
    Vector4 m_handleColor = Vector4(1.f, 1.f, 1.f, 1.f);
    std::function<void()> m_onValueChanged;
};

