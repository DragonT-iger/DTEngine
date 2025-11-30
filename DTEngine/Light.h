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


public:


	// 인스펙터 노출 변수
    int m_type = 0; // 0: Directional, 1: Point
    Vector3 m_color = { 1.0f, 1.0f, 1.0f };
    float m_intensity = 1.0f;
    float m_range = 10.0f; 

private:
    static std::vector<Light*> s_allLights;
};