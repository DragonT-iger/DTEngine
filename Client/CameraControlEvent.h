//#pragma once
//#include "MonoBehaviour.h"
//
//class Camera;
//
//class CameraControlEvent : public MonoBehaviour
//{
//		DTGENERATED_BODY(CameraControlEvent);
//public:
//    void Start() override;
//    void Update(float deltaTime) override;
//
//    void HandleMovement(float deltaTime);
//    void HandleZoom(float deltaTime);
//    
//    void SetCameraMoveSpeed(float speed) { m_moveSpeed = speed; }
//    const float& GetCameraMoveSpeed() const { return m_moveSpeed; }
//
//    void SetCameraMinX(float speed) { m_moveSpeed = speed; }
//    const float& GetCameraMinX() const { return m_moveSpeed; }
//
//    void SetCameraMaxX(float speed) { m_moveSpeed = speed; }
//    const float& GetCameraMaxX() const { return m_moveSpeed; }
//
//    void SetCameraZoomSpeed(float speed) { m_moveSpeed = speed; }
//    const float& GetCameraZoomSpeed() const { return m_moveSpeed; }
//
//
//
//private:
//		Camera* m_camera = nullptr;
//
//    // 카메라 이동.
//    float m_moveSpeed = 10.0f;
//    float m_minX = -50.0f;
//    float m_maxX = 50.0f;
//
//    // 줌 기능
//    float m_zoomSpeed = 5.0f;
//    float m_minZoom = 5.0f;
//    float m_maxZoom = 50.0f;
//};
//
