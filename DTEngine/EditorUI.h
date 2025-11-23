#pragma once
#include <memory>

#include <any>

#include "SimpleMathHelper.h"
#include "Game.h"

class Scene;
class GameObject;
class Component;
class Transform;
class RenderTexture;
class Camera;




class EditorUI
{
public:
    EditorUI();
    ~EditorUI();

    void RenderToolbar(Game::EngineMode currentMode, std::function<void(Game::EngineMode)> onModeChanged);

    void Render(Scene* activeScene);

    void RenderSceneWindow(RenderTexture* rt, Scene* scene , Camera* camera);
    void RenderGameWindow(RenderTexture* rt, Scene* scene);

    void ClearSelection() { m_selectedGameObject = nullptr; }
private:
    void DrawHierarchyWindow(Scene* activeScene);

    void DrawInspectorWindow();

    void DrawGizmo(Scene* activeScene, Camera* camera);

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