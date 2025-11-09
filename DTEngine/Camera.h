#pragma once
#include "MonoBehaviour.h"
#include <SimpleMath.h>
#include "SimpleMathHelper.h"


using Matrix = DirectX::SimpleMath::Matrix;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;

class Camera : public MonoBehaviour
{
    DTGENERATED_BODY(Camera);
public:

    Camera();
    virtual ~Camera() = default;

    void Awake() override;
    void LateUpdate(float deltaTime) override;

    void SetProjectionPerspective(float fovY, float aspectRatio, float nearZ, float farZ);
    void SetProjectionOrthographic(float viewWidth, float viewHeight, float nearZ, float farZ);

    const Matrix& GetViewMatrix() const { return m_view; }
    const Matrix& GetProjectionMatrix() const { return m_projection; }

    void SetThisCameraToMain();

    const float& GetNearZ() { return m_nearZ; }
    void SetNearZ(float nearZ) {
        if (m_nearZ < 0.5) {
            m_nearZ = 0.5;
        }
        else {
            m_nearZ = nearZ;
        }
        
        m_dirtyProj = true;
    }

    const float& GetFarZ() { return m_farZ; }
    void SetFarZ(float farZ) {
        if (m_farZ < m_nearZ + 1) {
            m_farZ = m_nearZ + 1;
        }
        else {
            m_farZ = farZ;
        }

        m_dirtyProj = true;
    }

    const Vector4& GetClearColor() const { return m_clearColor; }
    void SetClearColor(const Vector4& color) { m_clearColor = color; }

    const float& GetEditorFovY() const { return m_editorFovY; }
    void SetEditorFovY(float editorFovY) { m_editorFovY = editorFovY; m_fovY = SimpleMathHelper::Deg2Rad(editorFovY);  m_dirtyProj = true; }

    void SetViewDirty() { m_dirtyView = true; }


private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    Matrix m_view;
    Matrix m_projection;

    float m_lastAspectRatio = -1.0f;

    float m_fovY = SimpleMathHelper::Deg2Rad(60.0f);
    float m_editorFovY = 60.0f;

    float m_nearZ = 0.1f;
    float m_farZ = 1000.0f;

    bool m_dirtyView = true;
    bool m_dirtyProj = true;

    Vector4 m_clearColor = { 0.2,0.2,0.2,0.2 };

};