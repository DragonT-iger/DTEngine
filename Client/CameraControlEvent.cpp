#include "CameraControlEvent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Camera.h"

BEGINPROPERTY(CameraControlEvent)

DTPROPERTY_ACCESSOR(CameraControlEvent, m_moveSpeed, GetCameraMoveSpeed, SetCameraMoveSpeed)
DTPROPERTY_ACCESSOR(CameraControlEvent, m_minZ, GetCameraMinZ, SetCameraMinZ)
DTPROPERTY_ACCESSOR(CameraControlEvent, m_maxZ, GetCameraMaxZ, SetCameraMaxZ)

DTPROPERTY_ACCESSOR(CameraControlEvent, m_zoomSpeed, GetCameraZoomSpeed, SetCameraZoomSpeed)
DTPROPERTY_ACCESSOR(CameraControlEvent, m_minZoom, GetZoomMinX, SetZoomMinX)
DTPROPERTY_ACCESSOR(CameraControlEvent, m_maxZoom, GetZoomMaxX, SetZoomMaxX)

ENDPROPERTY()

void CameraControlEvent::Start()
{
		if (!m_camera)
		{
				Scene* scene = SceneManager::Instance().GetActiveScene();
				m_camera = scene->GetMainCamera();
		}
}

void CameraControlEvent::Update(float deltaTime)
{
		HandleMovement(deltaTime);
		HandleZoom(deltaTime);
}

void CameraControlEvent::HandleMovement(float deltaTime)
{
    if (!m_camera)
        return;

    auto& input = InputManager::Instance();

    // 우클릭 홀드 중이 아니면 무시
    if (!input.GetKey(KeyCode::MouseRight))
        return;

    const MousePos& mouseDelta = input.GetMouseDelta();

    Transform* camTf = m_camera->GetComponent<Transform>();
    Vector3 currentPos = camTf->GetPosition();

    // 마우스 드래그 방향으로 카메라 이동
    Vector3 newPos = currentPos;
    newPos.z += mouseDelta.y * m_moveSpeed * deltaTime;

    // 경계 제한
    newPos.z = std::clamp(newPos.z, m_minZ, m_maxZ);

    camTf->SetPosition(newPos);
}

void CameraControlEvent::HandleZoom(float deltaTime)
{
    auto& input = InputManager::Instance();

    float wheelDelta = input.GetMouseWheelDelta();
    if (wheelDelta == 0)
        return;

    // Orthographic 카메라면 Size 조절
    if (m_camera->IsOrthographic())
    {
        float currentSize = m_camera->GetOrthographicSize();
        float newSize = currentSize - wheelDelta * m_zoomSpeed;
        newSize = std::clamp(newSize, m_minZoom, m_maxZoom);
        m_camera->SetOrthographicSize(newSize);
    }
    // Perspective 카메라면 Y 위치 조절
    else
    {
        Transform* camTf = m_camera->GetComponent<Transform>();
        Vector3 currentPos = camTf->GetPosition();

        Vector3 forward = camTf->Forward();

        // 휠 미는 경우 줌인
        Vector3 newPos = currentPos + (forward * wheelDelta * m_zoomSpeed);

        // y값 제한 범위 밖이면 줌 더이상 안하기.
        if (newPos.y < m_minZoom || newPos.y > m_maxZoom)
        {
            return;
        }

        camTf->SetPosition(newPos);
    }
}

