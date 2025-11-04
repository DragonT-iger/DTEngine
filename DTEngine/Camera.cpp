#include "pch.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene.h"
#include "DX11Renderer.h"
#include "SceneManager.h"

Camera::Camera()
    : m_view(Matrix::Identity), m_projection(Matrix::Identity)
{
}

void Camera::Awake()
{
    Scene* ownerScene = SceneManager::Instance().GetActiveScene();

    //if (ownerScene && ownerScene->GetMainCamera() == nullptr)
    //{
    //    ownerScene->_SetMainCamera(this);
    //}
}

void Camera::LateUpdate(float deltaTime)
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::SetProjectionPerspective(float fovY, float aspectRatio, float nearZ, float farZ)
{
    m_fovY = fovY;
    m_nearZ = nearZ;
    m_farZ = farZ;
    m_projection = Matrix::CreatePerspectiveFieldOfView(fovY, aspectRatio, nearZ, farZ);
    m_lastAspectRatio = aspectRatio;
}

void Camera::SetProjectionOrthographic(float viewWidth, float viewHeight, float nearZ, float farZ)
{
    m_projection = Matrix::CreateOrthographic(viewWidth, viewHeight, nearZ, farZ);
}

void Camera::UpdateViewMatrix()
{
    Transform* tf = GetComponent<Transform>();
    if (!tf)
    {
        return;
    }

    const Vector3& pos = tf->GetPosition();
    const Vector3& forward = tf->Forward();
    const Vector3& up = tf->Up();

    m_view = Matrix::CreateLookAt(pos, pos + forward, up);
}

void Camera::UpdateProjectionMatrix()
{
    float currentAspectRatio = DX11Renderer::Instance().GetAspectRatio();

    if (currentAspectRatio != m_lastAspectRatio)
    {
        m_projection = Matrix::CreatePerspectiveFieldOfView(m_fovY, currentAspectRatio, m_nearZ, m_farZ);
        m_lastAspectRatio = currentAspectRatio;
    }
}