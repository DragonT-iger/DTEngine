#include "pch.h" // 프로젝트 설정에 따라 필요할 수 있음
#include "CameraRotator.h"
#include "Transform.h"
#include "InputManager.h"
#include "TilemapGenerator.h"
#include "TilemapData.h"


BEGINPROPERTY(CameraRotator)

DTPROPERTY(CameraRotator, m_tilemapGenerator)

ENDPROPERTY()

using namespace DirectX::SimpleMath;

void CameraRotator::Start()
{
    UpdatePositionToCenter();

    Vector3 currentEuler = GetTransform()->GetEditorEuler();
    m_currentAngleY = currentEuler.y;
    m_targetAngleY = m_currentAngleY;
}

void CameraRotator::Update(float deltaTime)
{
    // 입력 처리
    HandleInput();

    if (std::abs(m_targetAngleY - m_currentAngleY) > 0.01f)
    {
        PerformRotation(deltaTime);
    }
    else
    {
        // 거의 다 왔으면 값 고정
        m_currentAngleY = m_targetAngleY;
        GetTransform()->SetRotationEuler(Vector3(0.0f, m_currentAngleY, 0.0f));
        m_isRotating = false;
    }
}

void CameraRotator::UpdatePositionToCenter()
{
    if (m_tilemapGenerator == nullptr) return;

    TilemapData* data = m_tilemapGenerator->GetMapData();
    if (data == nullptr) return;

    int width = data->GetWidth();
    int height = data->GetHeight();
    float tileSize = TilemapData::TILE_SIZE;

    float centerX = (width * tileSize) * 0.5f;
    float centerZ = (height * tileSize) * 0.5f;

    GetTransform()->SetPosition(Vector3(centerX, 0.0f, centerZ));
}

void CameraRotator::HandleInput()
{
    if (InputManager::Instance().GetKeyDown(KeyCode::Q))
    {
        m_targetAngleY -= 45.0f;
        m_isRotating = true;
    }
    else if (InputManager::Instance().GetKeyDown(KeyCode::E))
    {
        m_targetAngleY += 45.0f;
        m_isRotating = true;
    }
}

void CameraRotator::PerformRotation(float deltaTime)
{
    m_currentAngleY = Lerp(m_currentAngleY, m_targetAngleY, m_rotationSpeed * deltaTime);

    GetTransform()->SetRotationEuler(Vector3(0.0f, m_currentAngleY, 0.0f));
}

float CameraRotator::Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}