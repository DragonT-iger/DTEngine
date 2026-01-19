#include "pch.h"
#include "UIManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Canvas.h"
#include "UIButton.h"
#include "UISlider.h"
#include "Image.h"
#include "InputManager.h"
#include "DX11Renderer.h"

static Canvas* GetCanvasInHierarchy(Transform* start)
{
    Transform* current = start;
    while (current)
    {
        if (auto* canvas = current->_GetOwner()->GetComponent<Canvas>())
        {
            return canvas;
        }
        current = current->GetParent();
    }
    return nullptr;
}


UIManager::UIManager()
{
    m_layerOrders["Default"] = 0;
    m_layerOrders["Second"] = 20;
    m_layerOrders["Third"] = 30;
    m_layerOrders["Fourth"] = 40;
    m_layerOrders["Fifth"] = 50;
    m_layerOrders["Sixth"] = 60;
    m_layerOrders["Last"] = 100;
}


void UIManager::OnResize(float width, float height)
{
    m_lastWidth = width;
    m_lastHeight = height;

    m_viewportOrigin = Vector2(0.0f, 0.0f);
    m_viewportSize = Vector2(width, height);
}

void UIManager::UpdateLayout(Scene* scene, float width, float height)
{
    if (!scene || width <= 0.0f || height <= 0.0f) return;

    if (m_lastWidth != width || m_lastHeight != height)
    {
        m_lastWidth = width;
        m_lastHeight = height;
    }
}

void UIManager::UpdateInteraction(Scene* scene, float width, float height)
{
    if (!scene || width <= 0.0f || height <= 0.0f) return;

    // table에 object 없으면 처리안함.
    if (m_uiInteractables.empty()) return;

    Vector2 viewportOrigin = m_viewportOrigin;
    Vector2 viewportSize = m_viewportSize;

    // 마우스 포지션 가져오기.
    const MousePos& mousePos = InputManager::Instance().GetMousePosition();
    Vector2 mouseScreen = Vector2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    bool insideViewport = (mouseScreen.x >= viewportOrigin.x && mouseScreen.y >= viewportOrigin.y && mouseScreen.x <= viewportOrigin.x + viewportSize.x && mouseScreen.y <= viewportOrigin.y + viewportSize.y);

    Vector2 localMouse = mouseScreen - viewportOrigin;
    float viewportScaleX = (viewportSize.x > 0.0f) ? (width / viewportSize.x) : 1.0f;
    float viewportScaleY = (viewportSize.y > 0.0f) ? (height / viewportSize.y) : 1.0f;
    localMouse.x *= viewportScaleX;
    localMouse.y *= viewportScaleY;

    bool mouseDown = InputManager::Instance().GetKeyDown(KeyCode::MouseLeft);
    bool mouseHeld = InputManager::Instance().GetKey(KeyCode::MouseLeft);
    bool mouseUp = InputManager::Instance().GetKeyUp(KeyCode::MouseLeft);

    for (const auto& go : m_uiInteractables)
    {
        // canvas 없는지 먼저 체크.
        if (!m_canvas) break;

        if (!go || !go->IsActiveInHierarchy())
        {
            if (m_activeSlider && go == m_activeSlider->_GetOwner())
            {
                m_activeSlider = nullptr;
            }
            continue;
        }

        UIButton* button = go->GetComponent<UIButton>();
        UISlider* slider = go->GetComponent<UISlider>();

        // 일단 button, slider 만 input 체크해서 처리하기.
        if (!button && !slider) continue;

        Transform* tf = go->GetTransform();

        float canvasScale = m_canvas->GetScaleFactor(width, height);
        if (canvasScale <= 0.0f) canvasScale = 1.0f;

        Vector3 worldPos = tf->GetWorldPosition();
        Vector3 lossyScale = tf->GetLossyScale();
        Vector2 centerScreen = Vector2(width * 0.5f, height * 0.5f) + Vector2(worldPos.x * canvasScale, -worldPos.y * canvasScale);
        
        Vector2 size = Vector2(lossyScale.x * canvasScale, lossyScale.y * canvasScale);
        Vector2 halfSize = Vector2(size.x * 0.5f, size.y * 0.5f);
        Vector2 min = centerScreen - halfSize;
        Vector2 max = centerScreen + halfSize;

        bool hovered = insideViewport &&
            localMouse.x >= min.x && localMouse.x <= max.x &&
            localMouse.y >= min.y && localMouse.y <= max.y;
        
        // 버튼 처리.
        if (button)
        {
            //printf("button test");
            if (!button->GetInteractable()) continue;
            button->SetHovered(hovered);

            if (mouseDown && hovered)
            {
                button->SetPressed(true);
            }

            if (mouseUp)
            {
                if (button->IsPressed() && hovered)
                {
                    button->InvokeClick();
                }
                button->SetPressed(false);
            }
        }
        
        // slider 처리.
        if (slider)
        {
            if (!slider->GetInteractable()) continue;

            if (mouseDown && hovered)
            {
                m_activeSlider = slider;
            }

            if (mouseHeld && m_activeSlider == slider)
            {
                float widthSpan = max.x - min.x;
                if (widthSpan > 0.0f)
                {
                    float t = (localMouse.x - min.x) / widthSpan;
                    t = std::clamp(t, 0.0f, 1.0f);
                    float value = slider->GetMinValue() + t * (slider->GetMaxValue() - slider->GetMinValue());
                    slider->SetValue(value);
                }
            }

            if (mouseUp && m_activeSlider == slider)
            {
                m_activeSlider = nullptr;
            }
        }
    }

    if (mouseUp)
    {
        m_activeSlider = nullptr;
    }
}


void UIManager::SetViewportRect(const Vector2& origin, const Vector2& size)
{
    m_viewportOrigin = origin;
    m_viewportSize = size;
}

void UIManager::RegisterLayer(const std::string& name, int order)
{
    if (name.empty()) return;

    m_layerOrders[name] = order;
}

void UIManager::SetUIInteractables(std::vector<GameObject*> interactables)
{
    if (m_layerOrders.empty())
    {
        m_layerOrders["Default"] = 0;
        m_layerOrders["Last"] = 100;
    }
    m_uiInteractables = std::move(interactables);
    m_activeSlider = nullptr;
}

void UIManager::RegisterUIInteractable(GameObject* interactable)
{
    if (!interactable) return;

    auto it = std::find(m_uiInteractables.begin(), m_uiInteractables.end(), interactable);

    
    if (it == m_uiInteractables.end())
    {
        //printf("registeruiinteractable"); // 버튼 추가하고 tables에 들어가졌어.
        m_layerOrders["Default"] = 0;
        m_layerOrders["Last"] = 100;
        m_uiInteractables.push_back(interactable);
    }
}

// layerorder 없으면 -1 return. 
int UIManager::GetLayerOrder(const std::string& name) const
{
    auto it = m_layerOrders.find(name);
    if (it != m_layerOrders.end())
    {
        return it->second;
    }
    return -1;
}

