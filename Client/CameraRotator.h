#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h" 

class TilemapGenerator;
class Camera;

class CameraRotator : public MonoBehaviour
{
    DTGENERATED_BODY(CameraRotator)
public:
    CameraRotator() = default;
    virtual ~CameraRotator() = default;

    virtual void Start() override;
    virtual void Update(float deltaTime) override;

    void SetTilemapGenerator(TilemapGenerator* generator) { m_tilemapGenerator = generator; }

    // 설정값
    void SetSpeeds(float panSpeed, float rotLerpSpeed, float zoomSpeed) {
        m_panSpeed = panSpeed;
        m_rotationLerpSpeed = rotLerpSpeed;
        m_zoomSpeed = zoomSpeed;
    }

    // 드래그 감도 설정 (얼마나 끌어야 45도 돌지)
    void SetDragThreshold(float threshold) { m_dragThreshold = threshold; }

private:
    void UpdatePositionToCenter();
    void HandleInput(float deltaTime);
    void PerformTransformation(float deltaTime);
    float Lerp(float a, float b, float t);

private:
    TilemapGenerator* m_tilemapGenerator = nullptr;
    Camera* m_targetCamera = nullptr;

    // --- 회전 관련 ---
    float m_targetAngleY = 0.0f;
    float m_currentAngleY = 0.0f;
    float m_rotationLerpSpeed = 10.0f;

    // 스냅 드래그 변수
    float m_dragAccumulatorX = 0.0f; // 마우스 이동 누적값
    float m_dragThreshold = 100.0f;  // 이만큼 움직이면 45도 회전 (픽셀 단위)
    bool m_hasRotatedThisDrag = false;
    // --- 이동(패닝) 관련 ---
    float m_panSpeed = 8.0f;

    // --- 줌 관련 ---
    float m_currentOrthoSize = 5.0f;
    float m_targetOrthoSize = 5.0f;
    float m_zoomSpeed = 0.2f;
    float m_minSize = 1.0f;  // 최소 사이즈 (최대 확대)
    float m_maxSize = 4.0f; // 최대 사이즈 (최대 축소)


};