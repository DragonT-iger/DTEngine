#pragma once

#include "Singleton.h"
#include "SimpleMathHelper.h"
#include <string>
#include <unordered_map>

class Scene;

class UIManager : public Singleton<UIManager>
{
public:
    void OnResize(float width, float height);
    void UpdateLayout(Scene* scene, float width, float height);
    void UpdateInteraction(Scene* scene, float width, float height);
    void SetViewportRect(const Vector2& origin, const Vector2& size);
    void RegisterLayer(const std::string& name, int order);
    int GetLayerOrder(const std::string& name) const;

private:
    void EnsureDefaultLayers();

    float m_lastWidth = 0.0f;
    float m_lastHeight = 0.0f;
    Vector2 m_viewportOrigin = Vector2(0.0f, 0.0f);
    Vector2 m_viewportSize = Vector2(0.0f, 0.0f);
    std::unordered_map<std::string, int> m_layerOrders;
};
