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

    printf("[UISliderHandle] Awake called\n");
}

void UISliderHandle::Update(float deltaTime)
{
    if (!m_parentSlider || !m_transform)
    {
        printf("[UISliderHandle] NULL check failed\n");
        return;
    }
    
    // InputManager에서 마우스 위치 가져오기
    const MousePos& mousePos = InputManager::Instance().GetGameMousePosition();
    Vector2 mousePosVec2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));


    Vector3 handleWorldPos = m_transform->GetWorldPosition();
    printf("handleWorldPos : %.1f, %.1f\n", handleWorldPos.x, handleWorldPos.y);

    // 마우스 왼쪽 버튼 Down
    if (InputManager::Instance().GetKeyDown(KeyCode::MouseLeft))
    {
        printf("\n=== Mouse Left Button DOWN ===\n");
        printf("Mouse Pos: %.1f, %.1f\n", mousePosVec2.x, mousePosVec2.y);
        Vector3 handleWorldPos = m_transform->GetWorldPosition();
        Vector3 handleScale = m_transform->GetScale();
        printf("Handle World Pos: %.3f, %.3f, %.3f\n",
            handleWorldPos.x, handleWorldPos.y, handleWorldPos.z);
        printf("Handle Scale: %.3f, %.3f, %.3f\n",
            handleScale.x, handleScale.y, handleScale.z);
        bool mouseOver = IsMouseOver(mousePosVec2);
        printf("IsMouseOver: %s\n", mouseOver ? "TRUE" : "FALSE");

        if (IsMouseOver(mousePosVec2))
        {
            printf(">>> DRAG START <<<\n");
            
            m_isDragging = true;
            m_dragStartPos = mousePosVec2;
            m_handleStartPos = m_transform->GetPosition();
        }
        else
        {
            printf(">>> DRAG END <<<\n");
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

void UISliderHandle::OnPointerDown(const Vector2& mousePos)
{
    if (!m_parentSlider || !m_parentSlider->GetInteractable()) return;

    m_isDragging = true;

    // 현재 핸들 월드 위치와 마우스 위치 차이 계산
    Vector3 handleWorldPos = m_transform->GetWorldPosition();
    m_dragOffset = mousePos.x - handleWorldPos.x;
}

void UISliderHandle::OnPointerDrag(const Vector2& mousePos)
{
    if (!m_isDragging || !m_parentSlider) return;

    // 마우스 위치를 부모(슬라이더) 로컬 좌표로 변환
    float mouseLocalX = mousePos.x - m_dragOffset;

    m_parentSlider->OnHandleDragged(mouseLocalX);
}

void UISliderHandle::OnPointerUp(const Vector2& mousePos)
{
    m_isDragging = false;
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
