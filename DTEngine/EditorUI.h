#pragma once
#include <memory>
#include <SimpleMath.h>

class Scene;
class GameObject;
class Component;
class Transform;

using Vector3 = DirectX::SimpleMath::Vector3;

class EditorUI
{
public:
    EditorUI();
    ~EditorUI();

    void Render(Scene* activeScene);

private:
    void DrawHierarchyWindow(Scene* activeScene);

    void DrawInspectorWindow();

    void DrawGizmo(Scene* activeScene);

    void DrawHierarchyNode(Transform* tf);

    void DrawComponentProperties(Component* comp);

    GameObject* m_selectedGameObject = nullptr;

    GameObject* m_lastSelectedGameObject = nullptr;
    Vector3 m_cachedEulerRotation;
};