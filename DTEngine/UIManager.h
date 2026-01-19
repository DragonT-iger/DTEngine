#pragma once

#include "Singleton.h"
#include "SimpleMathHelper.h"
#include "Canvas.h"
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

    // 캔버스 생성하면 UIManager에 넣어주자.
    void SetCanvas(const Canvas& canvas) { m_Canvas = &canvas; };

private:
    void EnsureDefaultLayers();

    float m_lastWidth = 0.0f;
    float m_lastHeight = 0.0f;
    Vector2 m_viewportOrigin = Vector2(0.0f, 0.0f);
    Vector2 m_viewportSize = Vector2(0.0f, 0.0f);

    std::unordered_map<std::string, int> m_layerOrders;

    const Canvas* m_Canvas = nullptr;
};
