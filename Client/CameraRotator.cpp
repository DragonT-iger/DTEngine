#include "pch.h"
#include "CameraRotator.h"
#include "Transform.h" 
#include "InputManager.h"
#include "TilemapGenerator.h"
#include "TilemapData.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Scene.h"

BEGINPROPERTY(CameraRotator)

DTPROPERTY(CameraRotator, m_tilemapGenerator)


ENDPROPERTY()

using namespace DirectX::SimpleMath;

void CameraRotator::Start()
{
    m_targetCamera = SceneManager::Instance().GetActiveScene()->GetMainCamera();
    UpdatePositionToCenter();

    Vector3 currentEuler = GetTransform()->GetEditorEuler();
    m_currentAngleY = currentEuler.y;
    m_targetAngleY = m_currentAngleY;

    if (m_targetCamera)
    {
        // 카메라 Size 초기화 (OrthographicSize 또는 Scale.x 사용)
        m_currentOrthoSize = m_targetCamera->GetOrthographicSize(); 
        m_targetOrthoSize = m_currentOrthoSize;
    }
}

void CameraRotator::Update(float deltaTime)
{
    HandleInput(deltaTime);
    PerformTransformation(deltaTime);
}

void CameraRotator::HandleInput(float deltaTime)
{
    InputManager& input = InputManager::Instance();

    if (input.GetKey(KeyCode::MouseLeft))
    {
        if (!m_hasRotatedThisDrag)
        {
            float deltaX = input.GetMouseDelta().x;
            m_dragAccumulatorX += deltaX; // 이동량 누적

            // 임계값을 넘으면 1회 회전 수행
            if (std::abs(m_dragAccumulatorX) >= m_dragThreshold)
            {
                if (m_dragAccumulatorX > 0)
                    m_targetAngleY += 45.0f;
                else
                    m_targetAngleY -= 45.0f;

                // ★ 중요: 회전 완료 플래그를 켜서 추가 회전 방지
                m_hasRotatedThisDrag = true; 
                
                // 누적값 초기화
                m_dragAccumulatorX = 0.0f; 
            }
        }
    }
    else
    {
        // 손을 떼면 누적값 초기화 (중간에 멈추면 취소)
        m_hasRotatedThisDrag = false;
        m_dragAccumulatorX = 0.0f;
    }

    // ---------------------------------------------------------
    // 2. 우클릭 드래그로 화면 잡아끄는 이동 (Grabbing / Panning)
    // ---------------------------------------------------------
    if (input.GetKey(KeyCode::MouseRight))
    {
        // 마우스 델타값
        float deltaX = input.GetMouseDelta().x;
        float deltaY = input.GetMouseDelta().y;

        Vector3 forward = m_targetCamera->_GetOwner()->GetTransform()->Forward();
        Vector3 right = m_targetCamera->_GetOwner()->GetTransform()->Right();

        forward.y = 0.0f; forward.Normalize();
        right.y = 0.0f;   right.Normalize();

        Vector3 moveDir = (right * -deltaX) + (forward * deltaY);

        // ★ 핵심 3: 계산된 벡터에서 Z 성분만 추출하여 적용
        // X 성분은 무시하고, Z 성분만 기존 위치에 더함
        Vector3 pos = GetTransform()->GetPosition();
        pos.z += moveDir.z * m_panSpeed * deltaTime;

        // pos.x는 건드리지 않음 (X축 이동 무시)
        GetTransform()->SetPosition(pos);
    }
    // ---------------------------------------------------------
    // 3. 줌 (휠)
    // ---------------------------------------------------------
    float wheel = input.GetMouseWheelDelta();
    if (wheel != 0.0f)
    {
        m_targetOrthoSize -= wheel * m_zoomSpeed;

        // 사이즈 제한 (너무 작아지거나 커지지 않게)
        if (m_targetOrthoSize < m_minSize) m_targetOrthoSize = m_minSize;
        if (m_targetOrthoSize > m_maxSize) m_targetOrthoSize = m_maxSize;
    }
}

void CameraRotator::PerformTransformation(float deltaTime)
{
    // 1. 회전 보간
    m_currentAngleY = Lerp(m_currentAngleY, m_targetAngleY, m_rotationLerpSpeed * deltaTime);
    GetTransform()->SetRotationEuler(Vector3(0.0f, m_currentAngleY, 0.0f));

    // 2. 줌 보간 (OrthographicSize 적용)
    if (m_targetCamera)
    {
        m_currentOrthoSize = Lerp(m_currentOrthoSize, m_targetOrthoSize, m_rotationLerpSpeed * deltaTime);

        // [핵심] Orthographic 카메라의 Size 속성 변경
        m_targetCamera->SetOrthographicSize(m_currentOrthoSize);
    }
}

// ... (UpdatePositionToCenter, Lerp는 이전과 동일)
void CameraRotator::UpdatePositionToCenter()
{
    if (!m_tilemapGenerator) return;
    auto* data = m_tilemapGenerator->GetMapData();
    if (!data) return;

    float cx = data->GetWidth() * TilemapData::TILE_SIZE * 0.5f;
    float cz = data->GetHeight() * TilemapData::TILE_SIZE * 0.5f;
    GetTransform()->SetPosition(Vector3(cx, 0.0f, cz));
}

float CameraRotator::Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}