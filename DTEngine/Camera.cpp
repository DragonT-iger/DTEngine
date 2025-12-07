#include "pch.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene.h"
#include "DX11Renderer.h"
#include "SceneManager.h"
#include "SimpleMathHelper.h"

BEGINPROPERTY(Camera)

DTPROPERTY_ACCESSOR(Camera, m_editorFovY , GetEditorFovY , SetEditorFovY)
DTPROPERTY_ACCESSOR(Camera, m_nearZ, GetNearZ , SetNearZ)
DTPROPERTY_ACCESSOR(Camera, m_farZ , GetFarZ , SetFarZ)
DTPROPERTY_ACCESSOR(Camera, m_clearColor, GetClearColor, SetClearColor)

ENDPROPERTY()




Camera::Camera()
    : m_view(SimpleMathHelper::IdentityMatrix()), m_projection(SimpleMathHelper::IdentityMatrix())
{
}

void Camera::Awake()
{
    SetThisCameraToMain();

    m_editorFovY = SimpleMathHelper::Rad2Deg(m_fovY);

    UpdateViewMatrix();
    UpdateProjectionMatrix();

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
    m_projection = DirectX::XMMatrixPerspectiveFovLH(m_fovY, m_aspectRatio, m_nearZ, m_farZ);
}

void Camera::SetProjectionOrthographic(float viewWidth, float viewHeight, float nearZ, float farZ)
{
    //m_projection = Matrix::CreateOrthographic(viewWidth, viewHeight, nearZ, farZ); 이것도 RH임

	m_projection = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
}

void Camera::SetThisCameraToMain()
{
    Scene* scene = SceneManager::Instance().GetActiveScene();

    if (scene->GetMainCamera() == nullptr && this->GetName() != "EditorCamera55") {
        scene->SetMainCamera(this);
    }
    else {
        std::cout << "Two Camera exists in this Scene Ignore last camera Or double SetThisCamera" << std::endl;
    }

}



void Camera::UpdateViewMatrix()
{

    Transform* tf = GetComponent<Transform>();
    //if (!tf)
    //{
    //    return;
    //}

    const Matrix& worldMatrix = tf->GetWorldMatrix();

    const Vector3& worldPos = worldMatrix.Translation();
	const Vector3& worldForward = tf->Forward();
    // SimpleMath에서 앞방향이 -Z임 우리는 +Z이 앞방향이니까 Transform에서 구하는게 맞음
	// SimpleMath는 RH 좌표계 기준 

    const Vector3& worldUp = worldMatrix.Up();

    m_view = DirectX::XMMatrixLookAtLH(worldPos, worldPos + worldForward, worldUp);

}

void Camera::UpdateProjectionMatrix()
{
    if (!m_dirtyProj) return;

    //float currentAspectRatio = DX11Renderer::Instance().GetAspectRatio(); 
    // 이건 전체 화면의 해상비임 


    if (m_nearZ < 0.01f)
    {
        m_nearZ = 0.01f;
    }
    if (m_farZ < m_nearZ + 0.1f)
    {
        m_farZ = m_nearZ + 0.1f;
    }

    // 얘없음  0보다 작은거 들어가서 튕김


    m_projection = DirectX::XMMatrixPerspectiveFovLH(m_fovY, m_aspectRatio, m_nearZ, m_farZ);


    m_dirtyProj = false;
   
}