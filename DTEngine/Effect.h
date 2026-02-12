#pragma once
#include "MonoBehaviour.h"
#include "ConstantBuffers.h"

class Effect : public MonoBehaviour
{
    DTGENERATED_BODY(Effect);

public:
    Effect();
    virtual ~Effect() = default;

public:
    void Update(float dt) override;
public:

    // 렌더링 직전에 호출되어 상수버퍼를 업데이트하는 함수
    void BindEP();

    void SetProgress(float p) { m_EM.progress = p; }

    void SetEdgeColor(const Vector4& color) { m_EM.edgeColor = color; }

    void SetTimeMultiplier(float m) { m_EM.timeMultiplier = m; }

    float GetTimeMultiplier() const { return m_EM.timeMultiplier; }
    Vector4 GetEdgeColor() const { return m_EM.edgeColor; }
    float GetProgress() const { return m_EM.progress; }

    void SetEffectType(float type) { m_EM.effectType = type;  }

    void SetNoiseScale(float scale) { m_EM.noiseScale = scale; }
    void SetCameraInverrse(const Matrix mat) { m_EM.invcamerarotation = mat; }
    void SetAnimating(bool value) { m_isAnimating = value; }

    void Reset() { m_EM.progress = 0.0f; }
protected:
    EffectParams m_EM;
    bool m_isAnimating = true; // 에디터에서 체크박스로 제어 가능
    bool m_isDamaged = false;
    float m_Timer = 0.0f;


};