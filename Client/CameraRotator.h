#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h" 

class TilemapGenerator;

class CameraRotator : public MonoBehaviour
{
     DTGENERATED_BODY(CameraRotator); 

public:
    CameraRotator() = default;
    virtual ~CameraRotator() = default;

    virtual void Start() override;
    virtual void Update(float deltaTime) override;

    void SetTilemapGenerator(TilemapGenerator* generator) { m_tilemapGenerator = generator; }
    void SetRotationSpeed(float speed) { m_rotationSpeed = speed; }

private:
    void UpdatePositionToCenter();
    void HandleInput();
    void PerformRotation(float deltaTime);

    float Lerp(float a, float b, float t);

private:
    TilemapGenerator* m_tilemapGenerator = nullptr;

    float m_targetAngleY = 0.0f;  
    float m_currentAngleY = 0.0f; 
    float m_rotationSpeed = 10.0f;

    bool m_isRotating = false;
};