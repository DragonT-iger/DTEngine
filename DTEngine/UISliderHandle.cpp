#include "pch.h"
#include "UISliderHandle.h"
#include "UISlider.h"
#include "Transform.h"
#include "InputManager.h"

BEGINPROPERTY(UISliderHandle)
ENDPROPERTY()

void UISliderHandle::Awake()
{
    m_transform = GetComponent<Transform>();
}

void UISliderHandle::Update(float deltaTime)
{
    if (!m_parentSlider || !m_transform)
    {
        return;
    }
    
    // InputManager에서 마우스 위치 가져오기
    const MousePos& mousePos = InputManager::Instance().GetGameMousePosition();
    Vector2 mousePosVec2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    Vector3 handleWorldPos = m_transform->GetWorldPosition();

    // 마우스 왼쪽 버튼 Down
    if (InputManager::Instance().GetKeyDown(KeyCode::MouseLeft))
    {
        Vector3 handleWorldPos = m_transform->GetWorldPosition();
        Vector3 handleScale = m_transform->GetScale();
       
        if (IsMouseOver(mousePosVec2))
        {
            m_isDragging = true;
            m_dragStartPos = mousePosVec2;
            m_handleStartPos = m_transform->GetPosition();
        }
    }

    // 드래그 중
    if (m_isDragging && InputManager::Instance().GetKey(KeyCode::MouseLeft))
    {
        if (!m_parentSlider->GetInteractable())
        {
            m_isDragging = false;
            return;
        }

        // 마우스 이동 거리 계산
        float deltaX = mousePosVec2.x - m_dragStartPos.x;

        // 새로운 핸들 로컬 X 위치
        float newLocalX = m_handleStartPos.x + deltaX;

        m_parentSlider->OnHandleDragged(newLocalX);
    }

    // 마우스 왼쪽 버튼 Up
    if (InputManager::Instance().GetKeyUp(KeyCode::MouseLeft))
    {
        m_isDragging = false;
    }
}

bool UISliderHandle::IsMouseOver(const Vector2& mousePos)
{
    if (!m_transform) return false;

    // Handle의 월드 위치와 크기
    Vector3 worldPos = m_transform->GetWorldPosition();
    Vector3 scale = m_transform->GetScale();

    // AABB 충돌 검사
    float left = worldPos.x - scale.x * 0.5f;
    float right = worldPos.x + scale.x * 0.5f;
    float top = worldPos.y - scale.y * 0.5f;
    float bottom = worldPos.y + scale.y * 0.5f;

    bool result = (mousePos.x >= left && mousePos.x <= right &&
        mousePos.y >= top && mousePos.y <= bottom);
    if (result) {
        std::cout << "Mouse: " << mousePos.x << ", " << mousePos.y << std::endl;
        std::cout << "Handle: " << worldPos.x << ", " << worldPos.y << std::endl;
    }

    return result;
}
