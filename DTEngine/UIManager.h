#pragma once

#include "Singleton.h"
#include "SimpleMathHelper.h"
#include "Canvas.h"
#include <string>
#include <unordered_map>
#include <vector>

class Scene;
class GameObject;
class UISlider;

class UIManager : public Singleton<UIManager>
{
public:
    UIManager();
    ~UIManager() = default;

    void OnResize(float width, float height);
    void UpdateLayout(Scene* scene, float width, float height);
    void UpdateInteraction(Scene* scene, float width, float height);
    void SetViewportRect(const Vector2& origin, const Vector2& size);
    void RegisterLayer(const std::string& name, int order);
    int GetLayerOrder(const std::string& name) const;

    void SetUIInteractables(std::vector<GameObject*> interactables);
    void RegisterUIInteractable(GameObject* interactable);
    void ClearUIObjecttables() { m_uiInteractables.clear(); };

    // 캔버스 생성하면 UIManager에 넣어주자.
    void SetCanvas(Canvas* canvas) { m_canvas = canvas; };

private:
    float m_lastWidth = 0.0f;
    float m_lastHeight = 0.0f;
    Vector2 m_viewportOrigin = Vector2(0.0f, 0.0f);
    Vector2 m_viewportSize = Vector2(0.0f, 0.0f);

    std::unordered_map<std::string, int> m_layerOrders;

    Canvas* m_canvas = nullptr;
    UISlider* m_activeSlider = nullptr;

    std::vector<GameObject*> m_uiInteractables;
};
