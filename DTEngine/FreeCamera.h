#pragma once
#include "MonoBehaviour.h"
#include <DirectXTK/SimpleMath.h> 

using namespace DirectX::SimpleMath;

class FreeCamera : public MonoBehaviour
{
    DTGENERATED_BODY(FreeCamera);

public:
    FreeCamera();
    virtual ~FreeCamera();

    void Awake() override;
    void Update(float deltaTime) override;

    static void SetIsSceneHovered(bool hovered) { s_isSceneHovered = hovered; }

private:
    void HandleInput(float deltaTime);

    float m_moveSpeed = 10.0f;     
    float m_turboScale = 3.0f;     
    float m_rotationSpeed = 0.1f;  

    float m_yaw = 0.0f;   
    float m_pitch = 0.0f; 

    bool m_isControlling = false;

    inline static bool s_isSceneHovered = false;
};