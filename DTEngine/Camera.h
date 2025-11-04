#pragma once
#include "MonoBehaviour.h"
#include <SimpleMath.h>

using Matrix = DirectX::SimpleMath::Matrix;
using Vector3 = DirectX::SimpleMath::Vector3;

class Camera : public MonoBehaviour
{
public:
    Camera();
    virtual ~Camera() = default;

    void Awake() override;
    void LateUpdate(float deltaTime) override;

    void SetProjectionPerspective(float fovY, float aspectRatio, float nearZ, float farZ);
    void SetProjectionOrthographic(float viewWidth, float viewHeight, float nearZ, float farZ);

    const Matrix& GetViewMatrix() const { return m_view; }
    const Matrix& GetProjectionMatrix() const { return m_projection; }

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    Matrix m_view;
    Matrix m_projection;

    float m_lastAspectRatio = -1.0f;
    float m_fovY = DirectX::XMConvertToRadians(60.0f);
    float m_nearZ = 0.1f;
    float m_farZ = 1000.0f;
};