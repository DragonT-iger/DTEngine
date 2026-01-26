#pragma once
#include <memory>

#include <any>
#include <filesystem>

#include "SimpleMathHelper.h"
#include "Game.h"

class Scene;
class GameObject;
class Component;
class Transform;
class RenderTexture;
class Camera;
class Texture;
class UIManager;



class EditorUI
{
public:
    EditorUI();
    ~EditorUI();

    void RenderToolbar(Game::EngineMode currentMode, std::function<void(Game::EngineMode)> onModeChanged);

    void Render(Scene* activeScene , Game::EngineMode engineMode);

    void RenderSceneWindow(RenderTexture* rt, Scene* scene , Camera* camera);
    void RenderGameWindow(RenderTexture* rt, Scene* scene);

    void DrawProjectWindow(Game::EngineMode engineMode);

    void DrawOverlay();

    void ClearSelection() { m_selectedGameObject = nullptr; }

    GameObject* GetSelectedGameObject() const { return m_selectedGameObject; }
    void SetSelectedGameObject(GameObject* go) { m_selectedGameObject = go; }

    Vector2 GetSceneViewportSize() const { return m_sceneViewportSize; }
    Vector2 GetGameViewportSize() const { return m_gameViewportSize; }

private:
    void DrawHierarchyWindow(Scene* activeScene);

    void DrawInspectorWindow();

    void DrawGizmo(Scene* activeScene, Camera* camera);

    void DrawHierarchyNode(Transform* tf);

    void DrawComponentProperties(Component* comp);

    void DrawEditorSettings();

    void AlignWithView();

    void OnDropFile(const std::string& filePath);

	  void DrawAssetInspector(const std::string& path);

    void CreatePrimitive(const std::string& name, const std::string& assetPath);
    UIManager* GetCachedUIManager(Scene* scene);    // uimanager 캐시해서 재사용하기 위해서.
    UIManager* m_cachedUIManager = nullptr;
    Scene* m_cachedUIManagerScene = nullptr;

    std::any m_dragStartValue;
    bool m_isGizmoUsing = false;
    Vector3 m_gizmoDragStartPosition;
    Quaternion m_gizmoDragStartRotation;
    Vector3 m_gizmoDragStartScale;

    Vector3 m_editorDragStartRotation;

    bool m_isInspectorLocked = false;
    GameObject* m_lockedGameObject = nullptr;
    std::string m_lockedAssetPath;


    GameObject* m_selectedGameObject = nullptr;

    std::vector<std::unique_ptr<GameObject>> m_clipboardGameObjects;
    std::unique_ptr<Component> m_clipboardComponent = nullptr;

    Camera* m_sceneCamera = nullptr;

    GameObject* m_lastSelectedGameObject = nullptr;
    Vector3 m_cachedEulerRotation;

    std::filesystem::path m_currentProjectDirectory = "Assets";
    std::string m_selectedAssetPath;


    Texture* m_iconFolder = nullptr;
    Texture* m_iconFile = nullptr;
    Texture* m_iconModel = nullptr;
    Texture* m_iconMaterial = nullptr;
    Texture* m_iconTexture = nullptr;
    Texture* m_iconAudio = nullptr;

    Vector2 m_sceneViewportSize = Vector2(0.0f, 0.0f);
    Vector2 m_gameViewportSize = Vector2(0.0f, 0.0f);

    bool m_isInspectorFocused = false;

    bool m_showRenamePopup = false;
    std::string m_renameTargetFile; 
    char m_renameBuffer[256] = "";  

};