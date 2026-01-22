#include "pch.h"
#include "UISliderHandle.h"
#include "UISlider.h"
#include "InputManager.h"
#include "UIButton.h"
#include "Image.h"
#include "DX11Renderer.h"

BEGINPROPERTY(UISliderHandle)
ENDPROPERTY()

void UISliderHandle::Awake()
{
    
    if (!m_transform)
    {
        m_transform = GetComponent<Transform>();
    }
    if (!m_button)
    {
        m_button = AddComponent<UIButton>();
    }
}

void UISliderHandle::Update(float deltaTime)
{
    if (!m_parentSlider || !m_transform)
    {
        return;
    }
    
    float screenW;
    float screenH;

#ifdef _DEBUG
    MousePos gameRes = InputManager::Instance().GetGameResolution();
    screenW = static_cast<float>(gameRes.x);
    screenH = static_cast<float>(gameRes.y);
#else
    screenW = static_cast<float>(DX11Renderer::Instance().GetWidth());
    screenH = static_cast<float>(DX11Renderer::Instance().GetHeight());
#endif

    const float refW = DX11Renderer::Instance().GetRefWidth();
    const float refH = DX11Renderer::Instance().GetRefHeight();

    float scaleX = screenW / refW;
    float scaleY = screenH / refH;
    const MousePos& mousePos = InputManager::Instance().GetGameMousePosition();
    float uiMouseX = static_cast<float>(mousePos.x) / scaleX;
    float uiMouseY = static_cast<float>(mousePos.y) / scaleY;
    Vector2 mousePosVec2(uiMouseX, uiMouseY);

    // 마우스 0 미만 혹은 해상도 초과 체크를 위해서. 그냥 없어도 될거같고 솔직히. 
    bool isOutOfBounds = (mousePos.x < 0 || mousePos.x >= static_cast<int>(screenW) ||
        mousePos.y < 0 || mousePos.y >= static_cast<int>(screenH));


    if (!m_parentSlider->GetInteractable())
    {
        m_isDragging = false;
        return;
    }

    if (m_button && m_button->IsPressed() && !m_isDragging)
    {
        m_isDragging = true;
    }
    if (m_isDragging)
    {
        if (InputManager::Instance().GetKey(KeyCode::MouseLeft) || isOutOfBounds)
        {
            float localMouseX = mousePosVec2.x;
            /*if (auto* handleTransform = m_transform->GetTransform())
            {
                localMouseX = mousePosVec2.x - handleTransform->GetPosition().x;
            }*/
            m_parentSlider->OnHandleDragged(localMouseX);
            m_isDragging = false;
        }
        else if (InputManager::Instance().GetKeyUp(KeyCode::MouseLeft))
        {
            float localMouseX = mousePosVec2.x;
           /* if (auto* handleTransform = m_transform->GetTransform())
            {
                localMouseX = mousePosVec2.x - handleTransform->GetPosition().x;
            }*/
            m_parentSlider->OnHandleReleased(localMouseX);
        }
    }

   /* if (InputManager::Instance().GetKeyUp(KeyCode::MouseLeft))
    {
        if (m_isDragging)
        {
            float localMouseX = mousePosVec2.x;
            if (auto* handleTransform = m_transform->GetTransform())
            {
                localMouseX = mousePosVec2.x - handleTransform->GetPosition().x;
            }
            m_parentSlider->OnHandleReleased(localMouseX);
        }
        m_isDragging = false;
    }*/
}

bool UISliderHandle::IsMouseOver(const Vector2& mousePos)
{
    if (!m_transform) return false;

    // AABB 충돌 검사
    float left = mousePos.x - m_transform->GetScale().x * 0.5f;
    float right = mousePos.x + m_transform->GetScale().x * 0.5f;
    float top = mousePos.y - m_transform->GetScale().y * 0.5f;
    float bottom = mousePos.y + m_transform->GetScale().y * 0.5f;

    bool result = (mousePos.x >= left && mousePos.x <= right &&
        mousePos.y >= top && mousePos.y <= bottom);

    return result;
}
