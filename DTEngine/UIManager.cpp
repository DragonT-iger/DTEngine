#include "pch.h"
#include "UIManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "RectTransform.h"
#include "Canvas.h"
#include "UIButton.h"
#include "Image.h"
#include "InputManager.h"
#include "DX11Renderer.h"
#include <Windows.h>

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

    EnsureDefaultLayers();

    if (m_lastWidth != width || m_lastHeight != height)
    {
        m_lastWidth = width;
        m_lastHeight = height;
    }

    std::vector<RectTransform*> roots;
    std::vector<RectTransform*> canvasRoots;

    auto collectRectChildren = [&](Transform* parent, auto&& collectRef) -> void {
        if (!parent) return;

        for (Transform* child : parent->GetChildren())
        {
            if (!child) continue;
            if (auto* rect = child->_GetOwner()->GetComponent<RectTransform>())
            {
                canvasRoots.push_back(rect);
            }
            else
            {
                collectRef(child, collectRef);
            }
        }
        };

    const auto& gameObjects = scene->GetGameObjects();
    for (const auto& go : gameObjects)
    {
        if (!go || !go->IsActiveInHierarchy()) continue;

        if (go->GetComponent<Canvas>())
        {
            if (auto* rect = go->GetComponent<RectTransform>())
            {
                canvasRoots.push_back(rect);
            }
            else if (auto* tf = go->GetTransform())
            {
                collectRectChildren(tf, collectRectChildren);
            }
            continue;
        }

        RectTransform* rect = go->GetComponent<RectTransform>();
        if (!rect) continue;

        Transform* tf = rect->GetTransform();
        Transform* parent = tf ? tf->GetParent() : nullptr;
        RectTransform* parentRect = parent ? parent->_GetOwner()->GetComponent<RectTransform>() : nullptr;
        if (!parentRect)
        {
            roots.push_back(rect);
        }
    }

    if (!canvasRoots.empty())
    {
        for (RectTransform* root : canvasRoots)
        {
            root->ApplyLayoutRecursive(width, height);
        }
        return;
    }

    for (RectTransform* root : roots)
    {
        root->ApplyLayoutRecursive(width, height);
    }
}

void UIManager::UpdateInteraction(Scene* scene, float width, float height)
{
    if (!scene || width <= 0.0f || height <= 0.0f) return;

    Vector2 viewportOrigin = m_viewportOrigin;
    Vector2 viewportSize = m_viewportSize;

    if (viewportSize.x <= 0.0f || viewportSize.y <= 0.0f)
    {
        HWND hwnd = DX11Renderer::Instance().GetHwnd();
        RECT clientRect{};
        POINT originPoint{ 0, 0 };
        if (hwnd && ::GetClientRect(hwnd, &clientRect) && ::ClientToScreen(hwnd, &originPoint))
        {
            viewportOrigin = Vector2(static_cast<float>(originPoint.x), static_cast<float>(originPoint.y));
            viewportSize = Vector2(static_cast<float>(clientRect.right - clientRect.left),
                static_cast<float>(clientRect.bottom - clientRect.top));
        }
    }

    const MousePos& mousePos = InputManager::Instance().GetMousePosition();
    Vector2 mouseScreen = Vector2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    bool insideViewport = (mouseScreen.x >= viewportOrigin.x &&
        mouseScreen.y >= viewportOrigin.y &&
        mouseScreen.x <= viewportOrigin.x + viewportSize.x &&
        mouseScreen.y <= viewportOrigin.y + viewportSize.y);

    Vector2 localMouse = mouseScreen - viewportOrigin;
    float viewportScaleX = (viewportSize.x > 0.0f) ? (width / viewportSize.x) : 1.0f;
    float viewportScaleY = (viewportSize.y > 0.0f) ? (height / viewportSize.y) : 1.0f;
    localMouse.x *= viewportScaleX;
    localMouse.y *= viewportScaleY;

    bool mouseDown = InputManager::Instance().GetKeyDown(KeyCode::MouseLeft);
    bool mouseUp = InputManager::Instance().GetKeyUp(KeyCode::MouseLeft);

    const auto& gameObjects = scene->GetGameObjects();
    for (const auto& go : gameObjects)
    {
        if (!go || !go->IsActiveInHierarchy()) continue;

        UIButton* button = go->GetComponent<UIButton>();
        if (!button || !button->GetInteractable()) continue;

        Image* image = go->GetComponent<Image>();

        RectTransform* rect = go->GetComponent<RectTransform>();
        if (!rect) continue;

        Transform* tf = rect->GetTransform();
        Canvas* canvas = tf ? GetCanvasInHierarchy(tf) : nullptr;
        Vector2 referenceResolution = Vector2(width, height);
        if (canvas)
        {
            referenceResolution = canvas->GetReferenceResolution();
            if (referenceResolution.x <= 0.0f || referenceResolution.y <= 0.0f)
            {
                referenceResolution = Vector2(width, height);
            }
        }

        float scaleX = (referenceResolution.x > 0.0f) ? (width / referenceResolution.x) : 1.0f;
        float scaleY = (referenceResolution.y > 0.0f) ? (height / referenceResolution.y) : 1.0f;
        if (scaleX <= 0.0f) scaleX = 1.0f;
        if (scaleY <= 0.0f) scaleY = 1.0f;

        Vector2 center = rect->GetScreenPosition(width, height);
        Vector2 size = rect->GetSize();
        Vector2 halfSize = Vector2(size.x * 0.5f * scaleX, size.y * 0.5f * scaleY);
        Vector2 min = center - halfSize;
        Vector2 max = center + halfSize;

        bool hovered = insideViewport &&
            localMouse.x >= min.x && localMouse.x <= max.x &&
            localMouse.y >= min.y && localMouse.y <= max.y;

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
