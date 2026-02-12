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

    if (input.GetKey(KeyCode::MouseRight))
    {
        if (!m_hasRotatedThisDrag)
        {
            float deltaX = input.GetMouseDelta().x;
            m_dragAccumulatorX += deltaX;

            if (std::abs(m_dragAccumulatorX) >= m_dragThreshold)
            {
                if (m_dragAccumulatorX > 0)
                    m_targetAngleY += 45.0f;
                else
                    m_targetAngleY -= 45.0f;

                m_hasRotatedThisDrag = true; 
                
                m_dragAccumulatorX = 0.0f; 
            }
        }
    }
    else
    {
        m_hasRotatedThisDrag = false;
        m_dragAccumulatorX = 0.0f;
    }

    if (input.GetKey(KeyCode::MouseLeft))
    {
        float deltaX = input.GetMouseDelta().x;
        float deltaY = input.GetMouseDelta().y;

        Vector3 forward = m_targetCamera->_GetOwner()->GetTransform()->Forward();
        Vector3 right = m_targetCamera->_GetOwner()->GetTransform()->Right();

        forward.y = 0.0f; forward.Normalize();
        right.y = 0.0f;   right.Normalize();


        Vector3 moveDir = (right * -deltaX) + (forward * deltaY);

        float zoomFactor = m_currentOrthoSize / m_maxSize;

        Vector3 pos = GetTransform()->GetPosition();
        pos.z += moveDir.z * m_panSpeed * zoomFactor * deltaTime;

        if (pos.z < -10) {
            pos.z = -10;
        }
        if (pos.z > 50) {
            pos.z = 50;
        }

        GetTransform()->SetPosition(pos);
    }

    float wheel = input.GetMouseWheelDelta();
    if (wheel != 0.0f)
    {
        m_targetOrthoSize -= wheel * m_zoomSpeed;

        if (m_targetOrthoSize < m_minSize) m_targetOrthoSize = m_minSize;
        if (m_targetOrthoSize > m_maxSize) m_targetOrthoSize = m_maxSize;
    }
}

void CameraRotator::PerformTransformation(float deltaTime)
{
    m_currentAngleY = Lerp(m_currentAngleY, m_targetAngleY, m_rotationLerpSpeed * deltaTime);
    GetTransform()->SetRotationEuler(Vector3(0.0f, m_currentAngleY, 0.0f));

    if (m_targetCamera)
    {
        m_currentOrthoSize = Lerp(m_currentOrthoSize, m_targetOrthoSize, m_rotationLerpSpeed * deltaTime);

        m_targetCamera->SetOrthographicSize(m_currentOrthoSize);
    }
}

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