#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h" 

class TilemapGenerator;
class Camera;
class Image;
class CameraRotator : public MonoBehaviour
{
    DTGENERATED_BODY(CameraRotator)
public:
    CameraRotator() = default;
    virtual ~CameraRotator() = default;

    void Start() override;
    void Update(float deltaTime) override;

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

    float m_targetAngleY = 0.0f;
    float m_currentAngleY = 0.0f;
    float m_rotationLerpSpeed = 10.0f;

    float m_dragAccumulatorX = 0.0f; 
    float m_dragThreshold = 100.0f;  
    bool m_hasRotatedThisDrag = false;

    float m_panSpeed = 18.0f;

    float m_currentOrthoSize = 5.0f;
    float m_targetOrthoSize = 5.0f;
    float m_zoomSpeed = 0.2f;
    float m_minSize = 1.0f;         // 최소 사이즈 (최대 확대)
    float m_maxSize = 4.0f;         // 최대 사이즈 (최대 축소)

    GameObject* m_rightPSWindowBG = nullptr;
    GameObject* m_leftPSWindowBG = nullptr;
    GameObject* m_rightRSWindowBG = nullptr;
    GameObject* m_leftRSWindowBG = nullptr;
    Image* m_victoryWindowImage = nullptr;

    GameObject* m_settingWindow = nullptr;


};