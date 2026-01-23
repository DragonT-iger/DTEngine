#include "pch.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene.h"
#include "DX11Renderer.h"
#include "SceneManager.h"
#include "SimpleMathHelper.h"
#include "MonitorController.h"

BEGINPROPERTY(Camera)

DTPROPERTY_ACCESSOR(Camera, m_editorFovY, GetEditorFovY, SetEditorFovY)
DTPROPERTY_ACCESSOR(Camera, m_nearZ, GetNearZ, SetNearZ)
DTPROPERTY_ACCESSOR(Camera, m_farZ, GetFarZ, SetFarZ)
DTPROPERTY_ACCESSOR(Camera, m_clearColor, GetClearColor, SetClearColor)
DTPROPERTY_ACCESSOR(Camera, m_isOrthographic, IsOrthographic, SetIsOrthographic)
DTPROPERTY_ACCESSOR(Camera, m_orthographicSize, GetOrthographicSize, SetOrthographicSize)
DTPROPERTY_ACCESSOR(Camera, m_viewportRect, GetViewportRect, SetViewportRect)
//DTPROPERTY_ACCESSOR(Camera, m_enablePostProcess, IsPostProcessEnabled, SetPostProcessEnabled)

ENDPROPERTY()




Camera::Camera()
    : m_view(SimpleMathHelper::IdentityMatrix()), m_projection(SimpleMathHelper::IdentityMatrix())
{
}

void Camera::Awake()
{


    Scene* scene = SceneManager::Instance().GetActiveScene();
    if (this->GetName() == "MainCamera") {

        if (scene->GetMainCamera() == nullptr) {
            scene->SetMainCamera(this);
        }
    }

    if (this->GetName() == "EditorCamera55")
    {
        if (scene->GetMainCamera() == nullptr) {
            scene->SetEditorCamera(this);
        }
    }

    m_editorFovY = SimpleMathHelper::Rad2Deg(m_fovY);

    UpdateViewMatrix();
    if (m_isOrthographic) {
        SetProjectionOrthographic();
    }
    else {
        UpdateProjectionMatrix();
    }

}

void Camera::LateUpdate(float deltaTime)
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::SetProjectionPerspective()
{

    if (m_nearZ < 0.01f) m_nearZ = 0.01f;
    if (m_farZ < m_nearZ + 0.1f) m_farZ = m_nearZ + 0.1f;
    m_projection = DirectX::XMMatrixPerspectiveFovLH(m_fovY, m_aspectRatio, m_nearZ, m_farZ);
}

void Camera::SetProjectionOrthographic()
{
    //m_projection = Matrix::CreateOrthographic(viewWidth, viewHeight, nearZ, farZ); 이것도 RH임
    if (m_orthographicSize < 0.01f) {
        m_orthographicSize = 0.01f;
    }
    if(m_nearZ > -2000.f) {
        m_nearZ = -2000.f;
	}

    float orthoHeight = 10 * m_orthographicSize;    

    m_projection = DirectX::XMMatrixOrthographicLH(orthoHeight * m_aspectRatio, orthoHeight, m_nearZ, m_farZ);
}

void Camera::SetThisCameraToMain()
{
    
    //else {
    //    std::cout << "Two Camera exists in this Scene Ignore last camera Or double SetThisCamera" << std::endl;
    //}

}

void Camera::Bind()
{
    float targetWidth = 0.0f;
    float targetHeight = 0.0f;

    if (m_targetTexture)
    {
        m_targetTexture->Bind();

        targetWidth = static_cast<float>(m_targetTexture->GetWidth());
        targetHeight = static_cast<float>(m_targetTexture->GetHeight());

        float x = targetWidth * m_viewportRect.x;
        float y = targetHeight * m_viewportRect.y;
        float w = targetWidth * m_viewportRect.z;
        float h = targetHeight * m_viewportRect.w;

        m_targetTexture->SetViewport(x, y, w, h);
    }
    else
    {

        targetWidth = static_cast<float>(DX11Renderer::Instance().Width());
        targetHeight = static_cast<float>(DX11Renderer::Instance().Height());
    }

    
    
}

const Vector3& Camera::GetCamPos() const 
{
    Transform* tf = GetComponent<Transform>();

    return tf->GetWorldPosition();

}

const Vector3& Camera::GetCamFor() const
{
    Transform* tf = GetComponent<Transform>();
    return tf->Forward();
}

Ray Camera::ScreenPointToRay(float x, float y, float viewportW, float viewportH) const
{
    DirectX::SimpleMath::Viewport vp;
    vp.x = 0.0f;
    vp.y = 0.0f;
    vp.width = viewportW;
    vp.height = viewportH;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;

    Vector3 screenNear(x, y, 0.0f);
    Vector3 screenFar(x, y, 1.0f);

    Vector3 nearWorld = vp.Unproject(screenNear, m_projection, m_view, SimpleMathHelper::IdentityMatrix());
    Vector3 farWorld = vp.Unproject(screenFar, m_projection, m_view, SimpleMathHelper::IdentityMatrix());

    Vector3 dir = farWorld - nearWorld;
    dir.Normalize();

    return { nearWorld, dir };
}

void Camera::SetPostProcessEffect(PostProcessType type, bool enable)
{
    uint32_t flag = static_cast<uint32_t>(type);
    if (enable)
    {
        m_postProcessMask |= flag;
    }
    else
    {
        m_postProcessMask &= ~flag;
    }
}

bool Camera::IsEffectEnabled(PostProcessType type) const
{
    return (m_postProcessMask & static_cast<uint32_t>(type)) != 0;
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



    // 얘없음  0보다 작은거 들어가서 튕김


    if(m_isOrthographic) {
        SetProjectionOrthographic();
	}
    else {
        SetProjectionPerspective();
    }

    m_dirtyProj = false;
   
}