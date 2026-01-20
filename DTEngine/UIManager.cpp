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


void UIManager::OnResize(float width, float height)
{
    EnsureDefaultLayers();
    m_lastWidth = width;
    m_lastHeight = height;

    m_viewportOrigin = Vector2(0.0f, 0.0f);
    m_viewportSize = Vector2(width, height);
}

void UIManager::UpdateLayout(Scene* scene, float width, float height)
{
    if (!scene || width <= 0.0f || height <= 0.0f) return;
}

void UIManager::UpdateInteraction(Scene* scene, float width, float height)
{
    if (!scene || !m_canvas) return;

    // 마우스 좌표계 변환은 여기서만. 너무 헷갈린다 여러곳에서 하니까.
    //const MousePos& mousePos = InputManager::Instance().GetGameMouseDelta();
    //Vector2 mouseScreen = Vector2((float)mousePos.x, (float)mousePos.y);
    //Vector2 localMouse = mouseScreen - m_viewportOrigin;

  

    //bool mouseDown = InputManager::Instance().GetKeyDown(KeyCode::MouseLeft);
    //bool mouseHeld = InputManager::Instance().GetKey(KeyCode::MouseLeft);
    //bool mouseUp = InputManager::Instance().GetKeyUp(KeyCode::MouseLeft);
}


void UIManager::SetViewportRect(const Vector2& origin, const Vector2& size)
{
    m_viewportOrigin = origin;
    m_viewportSize = size;
}

void UIManager::RegisterLayer(const std::string& name, int order)
{
    EnsureDefaultLayers();
    if (name.empty()) return;

    m_layerOrders[name] = order;
}

int UIManager::GetLayerOrder(const std::string& name) const
{
    auto it = m_layerOrders.find(name);
    if (it != m_layerOrders.end())
    {
        return it->second;
    }
    return 0;
}

void UIManager::EnsureDefaultLayers()
{
    if (m_layerOrders.empty())
    {
        m_layerOrders["Default"] = 0;
        m_layerOrders["Last"] = 100;
    }
}
