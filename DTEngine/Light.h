#pragma once

#include <SimpleMath.h>
#include <vector>

#include "MonoBehaviour.h"


using Vector3 = DirectX::SimpleMath::Vector3;

enum LightType
{
    Directional = 0,
    Point = 1,
};


class Light : public MonoBehaviour
{
    DTGENERATED_BODY(Light);

public:
    Light();
    virtual ~Light();

    void OnEnable() override;
    void OnDisable() override;

    static const std::vector<Light*>& GetAllLights() { return s_allLights; }


    const float& GetIntensity() const { return m_intensity; }
    void SetIntensity(float val);

    const float& GetRange() const { return m_range; }
    void SetRange(float val);

    const Vector3& GetColor() const { return m_color; }
    void SetColor(Vector3 val); 

    const int& GetTypeInt() const { return (const int&)m_type; }
    void SetTypeInt(int val);

public:


	// 인스펙터 노출 변수
    int m_type = 0; // 0: Directional, 1: Point
    Vector3 m_color = { 1.0f, 1.0f, 1.0f };
    float m_intensity = 1.0f;
    float m_range = 10.0f; 

private:
    static std::vector<Light*> s_allLights;
};