#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class Camera;

class CameraControlEvent : public MonoBehaviour
{
		DTGENERATED_BODY(CameraControlEvent);
public:
    void Start() override;
    void Update(float deltaTime) override;

    void HandleMovement(float deltaTime);
    void HandleZoom(float deltaTime);
    
    void SetCameraMoveSpeed(float speed) { m_moveSpeed = speed; }
    const float& GetCameraMoveSpeed() const { return m_moveSpeed; }

    void SetCameraMinZ(float maxZ) { m_minZ = maxZ; }
    const float& GetCameraMinZ() const { return m_minZ; }

    void SetCameraMaxZ(float maxZ) { m_maxZ = maxZ; }
    const float& GetCameraMaxZ() const { return m_maxZ; }

    void SetCameraZoomSpeed(float speed) { m_zoomSpeed = speed; }
    const float& GetCameraZoomSpeed() const { return m_zoomSpeed; }

    void SetZoomMinX(float minX) { m_minZoom = minX; }
    const float& GetZoomMinX() const { return m_minZoom; }

    void SetZoomMaxX(float maxX) { m_maxZoom = maxX; }
    const float& GetZoomMaxX() const { return m_maxZoom; }

private:
		Camera* m_camera = nullptr;

    // 카메라 이동.
    float m_moveSpeed = 7.0f;
    float m_minZ = -20.0f;
    float m_maxZ = 20.0f;

    // 줌 기능
    float m_zoomSpeed = 3.0f;
    float m_minZoom = 10.0f;
    float m_maxZoom = 30.0f;
};

