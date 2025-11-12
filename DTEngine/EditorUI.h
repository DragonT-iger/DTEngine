#pragma once
#include <memory>

#include <any>

#include "SimpleMathHelper.h"

class Scene;
class GameObject;
class Component;
class Transform;





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





    std::any m_dragStartValue;
    bool m_isGizmoUsing = false;
    Vector3 m_gizmoDragStartPosition;
    Quaternion m_gizmoDragStartRotation;
    Vector3 m_gizmoDragStartScale;

    Vector3 m_editorDragStartRotation;



    GameObject* m_selectedGameObject = nullptr;

    GameObject* m_lastSelectedGameObject = nullptr;
    Vector3 m_cachedEulerRotation;
};