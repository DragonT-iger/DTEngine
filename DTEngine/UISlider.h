#pragma once

#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"
#include <functional>

class UISlider : public MonoBehaviour
{
    DTGENERATED_BODY(UISlider);

public:
    UISlider() = default;
    ~UISlider() override = default;

    void Awake() override;

    void SetMinValue(float value) { m_minValue = value; }
    const float& GetMinValue() const { return m_minValue; }

    void SetMaxValue(float value) { m_maxValue = value; }
    const float& GetMaxValue() const { return m_maxValue; }

    void SetValue(float value);
    const float& GetValue() const { return m_value; }

    void SetWholeNumbers(bool value) { m_wholeNumbers = value; }
    const bool& GetWholeNumbers() const { return m_wholeNumbers; }

    void SetInteractable(bool value) { m_interactable = value; }
    const bool& GetInteractable() const { return m_interactable; }

    void SetFillColor(const Vector4& value) { m_fillColor = value; }
    const Vector4& GetFillColor() const { return m_fillColor; }

    void SetTrackColor(const Vector4& value) { m_trackColor = value; }
    const Vector4& GetTrackColor() const { return m_trackColor; }

    void SetHandleColor(const Vector4& value) { m_handleColor = value; }
    const Vector4& GetHandleColor() const { return m_handleColor; }

    // callback 함수 저장.
    void SetOnValueChanged(std::function<void(float)> callback) { m_onValueChanged = std::move(callback); }
    
    // callback 함수 호출.
    void InvokeValueChanged();

    // handle 찾아서 저장.
    void CacheHandle();

private:
    void Update(float deltaTime) override;
    void ApplyTrackColor();
    void ApplyHandleColor();

    class Transform* m_Transform = nullptr;
    class Transform* m_handleTransform = nullptr;
    class Image* m_handleImage = nullptr;
    class Image* m_trackImage = nullptr;
    
    float m_minValue = 0.0f;
    float m_maxValue = 1.0f;
    float m_value = 0.0f;
    bool m_wholeNumbers = false;
    bool m_interactable = true;
    Vector4 m_fillColor = Vector4(1.f, 1.f, 1.f, 1.f);
    Vector4 m_trackColor = Vector4(0.4f, 0.4f, 0.4f, 1.f);
    Vector4 m_handleColor = Vector4(1.f, 1.f, 1.f, 1.f);
    std::function<void(float)> m_onValueChanged;
};

