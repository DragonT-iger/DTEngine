#include "pch.h"


#include <imgui.h>
#include <string>
#include <cctype>
#include <algorithm>
#include <fstream>

#include "EditorUI.h"
#include "Scene.h"
#include "GameObject.h"
#include "MonoBehaviour.h"
#include "Transform.h" 
#include "ReflectionDatabase.h"
#include "ImGuizmo.h"
#include "Camera.h"
#include "ChangePropertyCommand.h"
#include "ChangeTransformCommand.h"
#include "HistoryManager.h"
#include "ChangeEditorTransformCommand.h"
#include "ChangeParentCommand.h"
#include "ChangeComponentActiveCommand.h"
#include "ChangeGameObjectActiveCommand.h"
#include "CreateGameObjectCommand.h"
#include "ComponentFactory.h"
#include "AddComponentCommand.h"
#include "RemoveComponentCommand.h"
#include "DestroyGameObjectCommand.h"
#include "InputManager.h"
#include "RenderTexture.h"
#include "FreeCamera.h"
#include "Light.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "AssetDatabase.h"
#include "RectTransform.h"
#include "Canvas.h"
//#include "PasteGameObjectCommand.h"
#include "SerializationUtils.h"
#include "TilemapData.h"
#include "TilemapGenerator.h"
#include "Prefab.h"
#include "Image.h"
#include "FSMController.h"
#include "DX11Renderer.h"
#include "SoundManager.h"
namespace fs = std::filesystem;

static ImGuizmo::OPERATION m_currentOperation = ImGuizmo::TRANSLATE;
static ImGuizmo::MODE      m_currentMode      = ImGuizmo::LOCAL;

static bool  m_useSnap = false;            
static float m_snapTranslation  = 2.0f;     
static float m_snapRotation     = 15.0f;       
static float m_snapScale        = 2.0f;           

template<typename T>
void DrawSceneReference(EditorUI* editor, const char* label, T* currentVal, Component* targetComp,
    const std::function<void(void*, void*)>& setter,
    std::function<std::string(T*)> nameGetter,
    std::function<T* (Transform*)> dropConverter)
{
    std::string displayStr = "None";
    if (currentVal) displayStr = nameGetter(currentVal);

    float width = ImGui::CalcItemWidth();
    if (ImGui::Button(displayStr.c_str(), ImVec2(width, 0)))
    {
        if (currentVal)
        {
            if constexpr (std::is_same_v<T, GameObject>) editor->SetSelectedGameObject(currentVal);
            else if constexpr (std::is_base_of_v<Component, T>) editor->SetSelectedGameObject(currentVal->_GetOwner());
        }
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DRAG_ITEM"))
        {
            IM_ASSERT(payload->DataSize == sizeof(Transform*));
            Transform* draggedTf = *(Transform**)payload->Data;

            if (draggedTf)
            {
                T* newVal = dropConverter(draggedTf);
                if (newVal)
                {
                    T* oldVal = currentVal;
                    setter(targetComp, &newVal);

                    auto cmd = std::make_unique<ChangePropertyCommand<T*>>(
                        targetComp, setter, oldVal, newVal
                    );
                    HistoryManager::Instance().Do(std::move(cmd));
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("%s", label);
}



// 파일 에셋(Texture, Material 등)을 드래그 앤 드롭으로 할당하는 헬퍼
template<typename T>
void DrawAssetReference(EditorUI* editor, const char* label, T* currentVal, Component* targetComp,
    const std::function<void(void*, void*)>& setter,
    const std::vector<std::string>& validExtensions)
{
    std::string displayStr = "None";
    if (currentVal)
    {
        uint64_t id = currentVal->GetMeta().guid;
        std::string path = AssetDatabase::Instance().GetPathFromID(id);
        if (!path.empty()) displayStr = std::filesystem::path(path).filename().string();
        else displayStr = "Loaded Asset";
    }

    float width = ImGui::CalcItemWidth();
    if (ImGui::Button(displayStr.c_str(), ImVec2(width, 0)))
    {
        if (currentVal)
        {
            uint64_t id = currentVal->GetMeta().guid;
            std::string pathStr = AssetDatabase::Instance().GetPathFromID(id);
        }
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_FILE"))
        {
            const char* droppedPath = (const char*)payload->Data;
            std::string ext = std::filesystem::path(droppedPath).extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            bool isValid = false;
            for (const auto& validExt : validExtensions) {
                if (ext == validExt) { isValid = true; break; }
            }

            if (isValid)
            {
                T* newVal = ResourceManager::Instance().Load<T>(droppedPath);
                if (newVal)
                {
                    T* oldVal = currentVal;
                    setter(targetComp, &newVal);

                    auto cmd = std::make_unique<ChangePropertyCommand<T*>>(
                        targetComp, setter, oldVal, newVal
                    );
                    HistoryManager::Instance().Do(std::move(cmd));
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("%s", label);
}



void EditorUI::DrawEditorSettings()
{
    ImGui::Begin("Editor Settings"); 

    ImGui::Text("Gizmo Operation");
    if (ImGui::RadioButton("Translate (W)", m_currentOperation == ImGuizmo::TRANSLATE)) m_currentOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate (E)", m_currentOperation == ImGuizmo::ROTATE)) m_currentOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale (R)", m_currentOperation == ImGuizmo::SCALE)) m_currentOperation = ImGuizmo::SCALE;

    ImGui::Separator();

    ImGui::Text("Gizmo Mode");
    if (ImGui::RadioButton("Local", m_currentMode == ImGuizmo::LOCAL)) m_currentMode = ImGuizmo::LOCAL;
    ImGui::SameLine();
    if (ImGui::RadioButton("World", m_currentMode == ImGuizmo::WORLD)) m_currentMode = ImGuizmo::WORLD;

    ImGui::Separator();

    ImGui::Text("Snap Settings");
    ImGui::Checkbox("Enable Snap", &m_useSnap);

    if (m_useSnap)
    {
        ImGui::Indent();
        ImGui::InputFloat("Move Step", &m_snapTranslation, 0.1f, 1.0f, "%.2f");
        ImGui::InputFloat("Rotate Step (Deg)", &m_snapRotation, 1.0f, 10.0f, "%.1f");
        ImGui::InputFloat("Scale Step", &m_snapScale, 0.1f, 1.0f, "%.2f");
        ImGui::Unindent();
    }

    ImGui::End();
}


EditorUI::EditorUI() {

    auto& res = ResourceManager::Instance();

    m_iconFolder =      res.Load<Texture>("Assets/Editor/Icons/Folder.png");
    m_iconFile =        res.Load<Texture>("Assets/Editor/Icons/File.png");
    m_iconModel =       res.Load<Texture>("Assets/Editor/Icons/Model.png"); 
    m_iconMaterial =    res.Load<Texture>("Assets/Editor/Icons/Material.png"); 
    m_iconTexture =     res.Load<Texture>("Assets/Editor/Icons/Image.png");
    m_iconAudio =       res.Load<Texture>("Assets/Editor/Icons/Audio.png");
}
EditorUI::~EditorUI() = default;

void EditorUI::RenderToolbar(Game::EngineMode currentMode, std::function<void(Game::EngineMode)> onModeChanged)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    auto& colors = ImGui::GetStyle().Colors;
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(colors[ImGuiCol_ButtonHovered].x, colors[ImGuiCol_ButtonHovered].y, colors[ImGuiCol_ButtonHovered].z, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(colors[ImGuiCol_ButtonActive].x, colors[ImGuiCol_ButtonActive].y, colors[ImGuiCol_ButtonActive].z, 0.5f));

    if (ImGui::BeginMainMenuBar())
    {
        float buttonWidth = 50.0f;
        float buttonCount = 2.0f;
        float groupWidth = (buttonWidth * buttonCount) + (ImGui::GetStyle().ItemSpacing.x * (buttonCount - 1));

        float startPosX = (ImGui::GetContentRegionAvail().x * 0.5f) - (groupWidth * 0.5f);
        ImGui::SetCursorPosX(startPosX);

        bool isPlayPause = (currentMode == Game::EngineMode::Play || currentMode == Game::EngineMode::Pause);
        if (isPlayPause) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.7f, 1.0f, 1.0f));

        if (ImGui::Button(isPlayPause ? "Stop" : "Play", ImVec2(buttonWidth, 0)))
        {
          
            if (isPlayPause)
            {
                SoundManager::Instance().ShutDown(); // 또는 ShutDown()
            }
            onModeChanged(Game::EngineMode::Play);

        }

        if (isPlayPause) ImGui::PopStyleColor();

        ImGui::SameLine();

        bool isPause = (currentMode == Game::EngineMode::Pause);
        ImGui::BeginDisabled(currentMode == Game::EngineMode::Edit);
        if (ImGui::Button(isPause ? "Resume" : "Pause", ImVec2(buttonWidth, 0)))
        {
            onModeChanged(Game::EngineMode::Pause);
        }
        ImGui::EndDisabled();

        ImGui::EndMainMenuBar();
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(1); 
}

void EditorUI::Render(Scene* activeScene , Game::EngineMode engineMode)
{
    if (!activeScene) return;
    DrawHierarchyWindow(activeScene);
    DrawInspectorWindow();
    DrawProjectWindow(engineMode);
    //DrawGizmo(activeScene);
    
    DrawEditorSettings();

    // Redo Undo

    bool ctrlPressed =      InputManager::Instance().GetKey(KeyCode::Control);
    bool shiftPressed =     InputManager::Instance().GetKey(KeyCode::Shift);
    bool mouseRightPressed =InputManager::Instance().GetKey(KeyCode::MouseRight);

    bool zPressed_Down = InputManager::Instance().GetKeyDown(KeyCode::Z);
    bool yPressed_Down = InputManager::Instance().GetKeyDown(KeyCode::Y);
    bool cPressed_Down = InputManager::Instance().GetKeyDown(KeyCode::C);
    bool vPressed_Down = InputManager::Instance().GetKeyDown(KeyCode::V);
    bool sPressed_Down = InputManager::Instance().GetKeyDown(KeyCode::S);
   

    static bool showFPSCounter = true;
    
    showFPSCounter ^= InputManager::Instance().GetKeyDown(KeyCode::Home);

    if (showFPSCounter) {
        DrawOverlay();
    }

    


    // Undo (Ctrl + Z)
    if (ctrlPressed && !shiftPressed && zPressed_Down)
    {
        HistoryManager::Instance().Undo();
        //std::cout << "Undo Performed" << std::endl;
    }

    // Redo (Ctrl + Y) 또는 (Ctrl + Shift + Z)
    if ((ctrlPressed && yPressed_Down) || (ctrlPressed && shiftPressed && zPressed_Down))
    {
        HistoryManager::Instance().Redo();
        //std::cout << "Redo Performed" << std::endl;
    }


    if (ctrlPressed && shiftPressed && InputManager::Instance().GetKeyDown(KeyCode::F))
    {
        AlignWithView();
    }

    if (ctrlPressed && cPressed_Down)
    {
        if (m_selectedGameObject)
        {
            m_clipboardGameObjects = m_selectedGameObject->Clone();
        }
    }

    if (ctrlPressed && vPressed_Down)
    {
        if (!m_clipboardGameObjects.empty() && !m_isInspectorFocused)
        {
            GameObject* prototype = m_clipboardGameObjects[0].get();
            std::vector<std::unique_ptr<GameObject>> newObjects = prototype->Clone();

            if (!newObjects.empty())
            {
                GameObject* rootInstance = newObjects[0].get();

                //if (m_selectedGameObject)
                //{
                //    rootInstance->GetTransform()->SetParent(m_selectedGameObject->GetTransform());
                //}
                for (auto& obj : newObjects)
                {
                    activeScene->AddGameObject(std::move(obj));
                }

                m_selectedGameObject = rootInstance;

                std::cout << "[Editor] Pasted GameObject: " << rootInstance->GetName() << std::endl;
            }
        }
    }

    //Scene Save (CTAL + S), 


    if (ctrlPressed && sPressed_Down && !mouseRightPressed)
    {
        if (activeScene)
        {
            if (engineMode == Game::EngineMode::Edit)
            {
                std::string sceneName = activeScene->GetName();
                if (!sceneName.empty())
                {
                    std::string relativePath = "Scenes/" + sceneName + ".scene";

                    std::cout << "Saving scene (" << sceneName << ") to: " << relativePath << std::endl;

                    if (activeScene->SaveFile(relativePath))
                    {
                        //std::cout << "Scene save successful. 권용범 바보ㅋㅋ" << std::endl;
                        std::cout << "Scene save successful." << std::endl;
                        HistoryManager::Instance().MarkAsSaved();
                    }
                    else
                    {
                        std::cout << "Scene save FAILED." << std::endl;
                    }
                }
                else
                {
                    std::cout << "Cannot save: Scene name is empty." << std::endl;
                }
            }
            else {
                ImGui::OpenPopup("Save Warning");
                std::cout << "Cannot Save In PlayMode." << std::endl;
            }
        }
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Save Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Cannot Save In PlayMode.");
        ImGui::Separator();

        float availableWidth = ImGui::GetContentRegionAvail().x;
        float buttonWidth = 120.0f;
        ImGui::SetCursorPosX((availableWidth - buttonWidth) * 0.5f);

        if (ImGui::Button("OK", ImVec2(buttonWidth, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }

}

void EditorUI::DrawGizmo(Scene* activeScene, Camera* camera) {
    //Camera* camera = activeScene->GetMainCamera();

    if (camera)
    {
        Matrix viewMatrix = camera->GetViewMatrix();
        Matrix projMatrix = camera->GetProjectionMatrix();

        GameObject* selectedObject = m_selectedGameObject;
        if (selectedObject)
        {
            Transform* transform = selectedObject->GetTransform();

            Matrix worldMatrix = transform->GetWorldMatrix();

            if (ImGui::IsKeyPressed(ImGuiKey_W)) m_currentOperation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGuiKey_E)) m_currentOperation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed(ImGuiKey_R)) m_currentOperation = ImGuizmo::SCALE;

            //std::cout << "Rendering Gizmo for: " << selectedObject->GetName() << std::endl;

            float snap[3] = { 0.f, 0.f, 0.f };
            if (m_useSnap)
            {
                if (m_currentOperation == ImGuizmo::TRANSLATE) {
                    snap[0] = snap[1] = snap[2] = m_snapTranslation;
                }
                else if (m_currentOperation == ImGuizmo::ROTATE) {
                    snap[0] = m_snapRotation; 
                }
                else if (m_currentOperation == ImGuizmo::SCALE) {
                    snap[0] = snap[1] = snap[2] = m_snapScale;
                }
            }
            // ---------------------

            ImGuizmo::Manipulate(
                (float*)&viewMatrix,
                (float*)&projMatrix,
                m_currentOperation,
                m_currentMode,
                (float*)&worldMatrix,
                NULL,               
                m_useSnap ? snap : NULL 
            );

            bool isUsing = ImGuizmo::IsUsing();

            if (isUsing && !m_isGizmoUsing)
            {
                m_isGizmoUsing = true;
                m_gizmoDragStartPosition = transform->GetPosition();
                m_gizmoDragStartRotation = transform->GetRotationQuat();
                m_gizmoDragStartScale = transform->GetScale();
            }

            if (isUsing)
            {
                Matrix newLocalMatrix = worldMatrix;
                if (transform->GetParent())
                {
                    Matrix parentWorldInv = transform->GetParent()->GetWorldMatrix().Invert();
                    newLocalMatrix = worldMatrix * parentWorldInv;
                }

                Vector3 newPos, newScale;
                Quaternion newRot;
                newLocalMatrix.Decompose(newScale, newRot, newPos);

                transform->SetPosition(newPos);
                transform->SetRotationQuat(newRot);
                transform->SetScale(newScale);
            }

            if (!isUsing && m_isGizmoUsing)
            {
                m_isGizmoUsing = false;

                Vector3 newPos = transform->GetPosition();
                Quaternion newRot = transform->GetRotationQuat();
                Vector3 newScale = transform->GetScale();

                auto cmd = std::make_unique<ChangeTransformCommand>(
                    transform,
                    m_gizmoDragStartPosition, newPos,  
                    m_gizmoDragStartRotation, newRot,  
                    m_gizmoDragStartScale, newScale    
                );

                HistoryManager::Instance().Do(std::move(cmd));
            }
        }
    }
}

void EditorUI::DrawHierarchyNode(Transform* tf)
{
    if (!tf) return;
    GameObject* go = tf->_GetOwner();
    if (!go) return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (m_selectedGameObject == go)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (tf->GetChildren().empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool node_open = ImGui::TreeNodeEx((void*)go->_GetID(), flags, go->GetName().c_str());

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        m_selectedGameObject = go;

        if (m_sceneCamera)
        {
            Transform* camTf = m_sceneCamera->_GetOwner()->GetTransform();

            Vector3 targetPos = m_selectedGameObject->GetTransform()->GetWorldPosition();

			Vector3 camForward = camTf->Forward();

            float focusDist = 7.0f;

            Vector3 newCamPos = targetPos - (camForward * focusDist);

            camTf->SetPosition(newCamPos);
        }
    }


    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("HIERARCHY_DRAG_ITEM", &tf, sizeof(Transform*));
        ImGui::Text("%s (Reparenting)", go->GetName().c_str()); 
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DRAG_ITEM"))
        {
            IM_ASSERT(payload->DataSize == sizeof(Transform*));
            Transform* draggedTf = *(Transform**)payload->Data;
            Transform* newParent = tf;
            Transform* oldParent = draggedTf->GetParent();

            if (draggedTf && newParent != oldParent && draggedTf != newParent)
            {
                auto cmd = std::make_unique<ChangeParentCommand>(draggedTf, oldParent, newParent);
                HistoryManager::Instance().Do(std::move(cmd));
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        m_selectedGameObject = go;
    }

    if (node_open)
    {
        for (Transform* child : tf->GetChildren())
        {
            DrawHierarchyNode(child);
        }
        ImGui::TreePop();
    }
}

void EditorUI::DrawHierarchyWindow(Scene* activeScene)
{
    ImGui::Begin("Hierarchy");

    if (!activeScene) return;

    if (m_selectedGameObject)
    {
		//std::cout << ImGui::IsAnyItemActive() << std::endl;

        if (InputManager::Instance().GetKeyDown(KeyCode::Delete) &&
            !ImGui::IsAnyItemActive())
        {

            GameObject* objectToSelectNext = nullptr;
            Transform* currentTf = m_selectedGameObject->GetTransform(); 

            if (currentTf)
            {
                Transform* parentTf = currentTf->GetParent();

                if (parentTf)
                {
                    objectToSelectNext = parentTf->_GetOwner();
                }
                else
                {
                    std::vector<GameObject*> rootGameObjects;
                    for (const auto& go : activeScene->GetGameObjects()) 
                    {
                        if (go && go->GetTransform()->GetParent() == nullptr)
                        {
                            rootGameObjects.push_back(go.get());
                        }
                    }

                    int index = -1;
                    for (size_t i = 0; i < rootGameObjects.size(); ++i)
                    {
                        if (rootGameObjects[i] == m_selectedGameObject)
                        {
                            index = (int)i;
                            break;
                        }
                    }

                    if (index != -1)
                    {
                        if (index + 1 < (int)rootGameObjects.size())
                        {
                            objectToSelectNext = rootGameObjects[index + 1];
                        }
                        else if (index > 0)
                        {
                            objectToSelectNext = rootGameObjects[index - 1];
                        }
                    }
                }
            }
            if (m_lockedGameObject == m_selectedGameObject)
            {
                m_lockedGameObject = nullptr;
                m_isInspectorLocked = false;
            }

            auto cmd = std::make_unique<DestroyGameObjectCommand>(activeScene, m_selectedGameObject);
            HistoryManager::Instance().Do(std::move(cmd));

            m_selectedGameObject = objectToSelectNext;

        }
    }



    std::string sceneName = activeScene->GetName();

    if (HistoryManager::Instance().IsDirty())
    {
        sceneName += "*";
    }

    ImGuiTreeNodeFlags sceneFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    bool scene_node_open = ImGui::TreeNodeEx(sceneName.c_str(), sceneFlags);



    if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Create Empty"))
        {
            auto cmd = std::make_unique<CreateGameObjectCommand>(activeScene, "GameObject");
            CreateGameObjectCommand* pCmd = cmd.get();

            HistoryManager::Instance().Do(std::move(cmd));

            if (pCmd)
            {
                m_selectedGameObject = pCmd->GetCreatedGameObject();
            }
        }

        if (ImGui::BeginMenu("3D Object"))
        {
            if (ImGui::MenuItem("Cube"))
            {
                CreatePrimitive("Cube", "Assets/Models/Primitives/Cube.fbx");
            }
            if (ImGui::MenuItem("Sphere"))
            {
                CreatePrimitive("Sphere", "Assets/Models/Primitives/Sphere.fbx");
            }
            if (ImGui::MenuItem("Plane"))
            {
                CreatePrimitive("Plane", "Assets/Models/Primitives/Plane.fbx");
            }
            if(ImGui::MenuItem("Cylinder"))
            {
                CreatePrimitive("Cylinder", "Assets/Models/Primitives/Cylinder.fbx");
			}
            if(ImGui::MenuItem("Cone"))
            {
                CreatePrimitive("Cone", "Assets/Models/Primitives/Cone.fbx");
			}
            ImGui::EndMenu();
        }

        ImGui::EndPopup();


    }






    if (scene_node_open)
    {
        const auto& gameObjects = activeScene->GetGameObjects();
        for (const auto& go : gameObjects)
        {
            if (go)
            {
                if (go->HasFlag(GameObject::Flags::HideInHierarchy))
                    continue;

                Transform* tf = go->GetTransform();
                if (tf && tf->GetParent() == nullptr) 
                {
                    DrawHierarchyNode(tf);
                }
            }
        }
        ImGui::TreePop(); 
    }

    ImVec2 avail = ImGui::GetContentRegionAvail();

    if (avail.x < 1.0f) avail.x = 1.0f;

    if (avail.y < 50.0f) avail.y = 50.0f;

    if (ImGui::InvisibleButton("##HierarchyVoid", avail))
    {
        m_selectedGameObject = nullptr;
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DRAG_ITEM"))
        {
            IM_ASSERT(payload->DataSize == sizeof(Transform*));
            Transform* draggedTf = *(Transform**)payload->Data;
            Transform* oldParent = (draggedTf) ? draggedTf->GetParent() : nullptr;
            Transform* newParent = nullptr;

            if (draggedTf && oldParent != newParent)
            {
                auto cmd = std::make_unique<ChangeParentCommand>(draggedTf, oldParent, newParent);
                HistoryManager::Instance().Do(std::move(cmd));
            }
        }

        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_FILE"))
        {
            const char* filePath = (const char*)payload->Data;
            OnDropFile(filePath);
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}


void EditorUI::DrawInspectorWindow()
{
    ImGui::Begin("Inspector");

    m_isInspectorFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

    GameObject* targetGameObject = m_selectedGameObject;
    std::string targetAssetPath = m_selectedAssetPath;

    if (m_isInspectorLocked)
    {
        if (m_lockedGameObject)
        {
            targetGameObject = m_lockedGameObject;
        }
        else if (!m_lockedAssetPath.empty())
        {
            targetAssetPath = m_lockedAssetPath;
            targetGameObject = nullptr;
        }
    }

    if (targetGameObject)
    {
        bool oldState = targetGameObject->IsActive();
        bool newState = oldState;

        if (ImGui::Checkbox("##ActiveCheckbox", &newState))
        {
            auto cmd = std::make_unique<ChangeGameObjectActiveCommand>(targetGameObject, oldState, newState);
            HistoryManager::Instance().Do(std::move(cmd));
        }

        ImGui::SameLine();

        char nameBuffer[128];
        strncpy_s(nameBuffer, targetGameObject->GetName().c_str(), sizeof(nameBuffer) - 1);
        nameBuffer[sizeof(nameBuffer) - 1] = '\0';

        ImGui::SetNextItemWidth(150);
        if (ImGui::InputText("##NameInput", nameBuffer, sizeof(nameBuffer)))
        {
            targetGameObject->SetName(nameBuffer);
        }

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();

        char tagBuffer[128];
        strncpy_s(tagBuffer, targetGameObject->GetTag().c_str(), sizeof(tagBuffer) - 1);
        tagBuffer[sizeof(tagBuffer) - 1] = '\0';

        ImGui::SetNextItemWidth(80);
        if (ImGui::InputText("Tag", tagBuffer, sizeof(tagBuffer)))
        {
            targetGameObject->SetTag(tagBuffer);
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Lock", &m_isInspectorLocked))
        {
            if (m_isInspectorLocked)
            {
                m_lockedGameObject = targetGameObject;
                m_lockedAssetPath = targetAssetPath;
            }
            else
            {
                m_lockedGameObject = nullptr;
                m_lockedAssetPath.clear();

            }
        }


        ImGui::Separator();

        DrawComponentProperties(targetGameObject->GetTransform());

        for (const auto& comp : targetGameObject->_GetComponents())
        {
            if (comp) DrawComponentProperties(comp.get());
        }

        float totalWidth = ImGui::GetContentRegionAvail().x;
        float buttonWidth = totalWidth * 0.6f;
        float padding = totalWidth * 0.20f;

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::SetCursorPosX(padding);

        if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0)))
        {
            ImGui::OpenPopup("AddComponentPopup");
        }

        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            ImGui::Text("Component Type:");
            ImGui::Separator();

            auto& factory = ComponentFactory::Instance();
            std::vector<std::string> componentNames = factory.GetAllRegisteredTypeNames();

            std::sort(componentNames.begin(), componentNames.end());

            for (const std::string& typeName : componentNames)
            {
                if (typeName == "Transform") continue;

                if (ImGui::Selectable(typeName.c_str()))
                {
                    auto cmd = std::make_unique<AddComponentCommand>(targetGameObject, typeName);
                    HistoryManager::Instance().Do(std::move(cmd));

                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
    }
    else if (!targetAssetPath.empty())
    {
        DrawAssetInspector(targetAssetPath);
    }
    else
    {
        ImGui::Text("No object or asset selected.");
    }

    ImGui::End();
}

void EditorUI::DrawComponentProperties(Component* comp)
{
    if (!comp) return;

    ImGui::PushID(comp);

    const ClassInfo* info = ReflectionDatabase::Instance().GetClassInfomation(comp->_GetTypeName());
    if (!info) return;

    bool compIsActive = true;

    MonoBehaviour* mb = dynamic_cast<MonoBehaviour*>(comp);
    if (mb)
    {
        bool oldState = mb->IsActive();
        bool newState = oldState; 

        std::string chkID = "##active_";
        chkID += comp->_GetTypeName();

        if (ImGui::Checkbox(chkID.c_str(), &newState)) 
        {
            auto cmd = std::make_unique<ChangeComponentActiveCommand>(mb, oldState, newState);
            HistoryManager::Instance().Do(std::move(cmd));
        }
        ImGui::SameLine();
    }


    bool header_open = ImGui::CollapsingHeader(comp->_GetTypeName(), ImGuiTreeNodeFlags_DefaultOpen);

    bool isTransform = (dynamic_cast<Transform*>(comp) != nullptr);

    if (ImGui::BeginPopupContextItem("ComponentContextMenu"))
    {
        if (isTransform)
        {
            if (ImGui::MenuItem("Align With View"))
            {
                if (m_sceneCamera && comp->_GetOwner())
                {
                    Transform* targetTf = comp->GetTransform();
                    Transform* camTf = m_sceneCamera->GetTransform();

                    if (targetTf && camTf)
                    {
                        Vector3 oldPos = targetTf->GetPosition();
                        Quaternion oldRot = targetTf->GetRotationQuat();
                        Vector3 oldScale = targetTf->GetScale();

                        Vector3 newPos = camTf->GetPosition();
                        Quaternion newRot = camTf->GetRotationQuat();
                        Vector3 newScale = oldScale;

                        auto cmd = std::make_unique<ChangeTransformCommand>(
                            targetTf,
                            oldPos, newPos,
                            oldRot, newRot,
                            oldScale, newScale
                        );
                        HistoryManager::Instance().Do(std::move(cmd));
                    }
                }
            }
            if (ImGui::MenuItem("Reset"))
            {
                Transform* tf = static_cast<Transform*>(comp);

                Vector3 oldPos = tf->GetPosition();
                Quaternion oldRot = tf->GetRotationQuat();
                Vector3 oldScale = tf->GetScale();

                Vector3 newPos = Vector3(0, 0, 0);
                Quaternion newRot = Quaternion(0.f, 0.f, 0.f, 1.f);
                Vector3 newScale = Vector3(1, 1, 1);

                auto cmd = std::make_unique<ChangeTransformCommand>(
                    tf,
                    oldPos, newPos,
                    oldRot, newRot,
                    oldScale, newScale
                );
                HistoryManager::Instance().Do(std::move(cmd));
            }


            ImGui::MenuItem("Remove Component", NULL, false, false);
            ImGui::TextDisabled("Cannot remove Transform");
        }
        else
        {

            if (ImGui::MenuItem("Remove Component"))
            {
                if (m_selectedGameObject)
                {
                    auto cmd = std::make_unique<RemoveComponentCommand>(m_selectedGameObject, comp);
                    HistoryManager::Instance().Do(std::move(cmd));

                    ImGui::EndPopup();
                    ImGui::PopID();
                    return;
                }
            }
        }
        if (ImGui::MenuItem("Copy Component"))
        {
            m_clipboardComponent = ComponentFactory::Instance().Create(comp->_GetTypeName());

            if (m_clipboardComponent)
            {
                CopyComponentValues(comp, m_clipboardComponent.get());
            }
        }

        bool canPaste = (m_clipboardComponent != nullptr) &&
            (std::string(m_clipboardComponent->_GetTypeName()) == comp->_GetTypeName());

        if (ImGui::MenuItem("Paste Component Values", NULL, false, canPaste))
        {
            CopyComponentValues(m_clipboardComponent.get(), comp);
        }


        ImGui::EndPopup();
    }


    if (header_open)
    {
        for (const PropertyInfo& prop : info->m_properties)
        {
            if (prop.m_name == "m_editorEulerAngles" || prop.m_name == "m_parent")
            {
                continue;
            }
            ImGui::PushID(&prop);
            void* data = prop.m_getter(comp);
            const auto& type = prop.m_type;
            //const char* name = prop.m_name.c_str();


            std::string prettyName = prop.m_name;

            if (prettyName.length() > 2 && prettyName.find("m_") == 0)
            {
                prettyName = prettyName.substr(2); // 예: "m_position" -> "position"
            }

            if (!prettyName.empty())
            {
                prettyName[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(prettyName[0]))); // 예: "position" -> "Position"
            }

            const char* name = prettyName.c_str();
            //if (!prop.m_enumNames.empty() && type == typeid(int))
            //{
            //    int* valPtr = static_cast<int*>(data);
            //    int currentItem = *valPtr;

            //    std::vector<const char*> items;
            //    for (const auto& enumName : prop.m_enumNames)
            //        items.push_back(enumName.c_str());

            //    if (ImGui::Combo(name, &currentItem, items.data(), (int)items.size()))
            //    {
            //        prop.m_setter(comp, &currentItem);
            //    }

            //    if (ImGui::IsItemActivated()) m_dragStartValue = *valPtr;
            //    if (ImGui::IsItemDeactivatedAfterEdit())
            //    {
            //        int oldVal = std::any_cast<int>(m_dragStartValue);
            //        auto cmd = std::make_unique<ChangePropertyCommand<int>>(comp, prop.m_setter, oldVal, currentItem);
            //        HistoryManager::Instance().Do(std::move(cmd));
            //    }
            //}
            // float
            if (type == typeid(float))
            {
                float temp = *static_cast<float*>(data);
                if (ImGui::DragFloat(name, &temp, 0.1f))
                {
                    prop.m_setter(comp, &temp);
                }
                if (ImGui::IsItemActivated())
                {
                    m_dragStartValue = *static_cast<float*>(data);
                }
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    float oldVal = std::any_cast<float>(m_dragStartValue);
                    float newVal = temp;

                    auto cmd = std::make_unique<ChangePropertyCommand<float>>(
                        comp, prop.m_setter, oldVal, newVal
                    );
                    HistoryManager::Instance().Do(std::move(cmd));
                }
            }

            // int
            else if (type == typeid(int))
            {
                int temp = *static_cast<int*>(data);
                if (ImGui::DragInt(name, &temp))
                {
                    prop.m_setter(comp, &temp);
                }
                if (ImGui::IsItemActivated())
                {
                    m_dragStartValue = *static_cast<int*>(data);
                }
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    int oldVal = std::any_cast<int>(m_dragStartValue);
                    int newVal = temp;

                    auto cmd = std::make_unique<ChangePropertyCommand<int>>(
                        comp, prop.m_setter, oldVal, newVal
                    );
                    HistoryManager::Instance().Do(std::move(cmd));
                }
            }
            // bool
            else if (type == typeid(bool))
            {
                bool temp = *static_cast<bool*>(data);
                if (ImGui::Checkbox(name, &temp))
                {
                    bool oldVal = *static_cast<bool*>(data);
                    bool newVal = temp;
                    auto cmd = std::make_unique<ChangePropertyCommand<bool>>(
                        comp, prop.m_setter, oldVal, newVal
                    );
                    HistoryManager::Instance().Do(std::move(cmd));
                }
            }
            // uint64_t (ID 등)
            else if (type == typeid(uint64_t))
            {
                uint64_t currentID = *static_cast<uint64_t*>(data);

                std::string displayStr = "None";
                if (currentID != 0)
                {
                    std::string path = AssetDatabase::Instance().GetPathFromID(currentID);
                    if (!path.empty())
                        displayStr = std::filesystem::path(path).filename().string();
                    else
                        displayStr = "Unknown ID (" + std::to_string(currentID) + ")";
                }

                float width = ImGui::CalcItemWidth();

                //ImGui::Button(displayStr.c_str(), ImVec2(width, 0));

                if (ImGui::Button(displayStr.c_str(), ImVec2(width, 0)))
                {
                    if (currentID != 0)
                    {
                        std::string pathStr = AssetDatabase::Instance().GetPathFromID(currentID);
                        if (!pathStr.empty())
                        {
                            std::filesystem::path path(pathStr);

                            m_currentProjectDirectory = path.parent_path();

                            //m_selectedAssetPath = pathStr;
                            //m_selectedGameObject = nullptr; 
                        }
                    }
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_FILE"))
                    {
                        const char* droppedPath = (const char*)payload->Data;
                        std::filesystem::path fpath(droppedPath);
                        std::string ext = fpath.extension().string();

                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                        std::string propCheck = prettyName;
                        std::transform(propCheck.begin(), propCheck.end(), propCheck.begin(), ::tolower);

                        bool isFormatValid = false;

                        if (propCheck.find("model") != std::string::npos || propCheck.find("mesh") != std::string::npos)
                        {
                            if (ext == ".fbx" || ext == ".obj" || ext == ".x" || ext == ".glb") isFormatValid = true;
                        }
                        else if (propCheck.find("material") != std::string::npos)
                        {
                            if (ext == ".mat") isFormatValid = true;
                        }
                        else if (propCheck.find("texture") != std::string::npos || propCheck.find("image") != std::string::npos)
                        {
                            if (ext == ".png" || ext == ".jpg" || ext == ".dds" || ext == ".tga" || ext == ".bmp") isFormatValid = true;
                        }
                        else if (propCheck.find("shader") != std::string::npos)
                        {
                            if (ext == ".hlsl" || ext == ".fx") isFormatValid = true;
                        }
                        else
                        {
                            isFormatValid = true;
                        }

                        if (isFormatValid)
                        {
                            uint64_t newID = AssetDatabase::Instance().GetIDFromPath(droppedPath);
                            if (newID != 0)
                            {
                                uint64_t oldVal = currentID;
                                prop.m_setter(comp, &newID);
                                auto cmd = std::make_unique<ChangePropertyCommand<uint64_t>>(
                                    comp, prop.m_setter, oldVal, newID
                                );
                                HistoryManager::Instance().Do(std::move(cmd));
                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }


                

                ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
                ImGui::Text("%s", name);
            }
            // std::string
            else if (type == typeid(std::string))
            {
                std::string temp = *static_cast<std::string*>(data);
                char buffer[256]; // 임시 버퍼
                strncpy_s(buffer, temp.c_str(), sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0'; // 널 종료 보장

                if (ImGui::InputText(name, buffer, sizeof(buffer)))
                {
                    temp = buffer;
                    prop.m_setter(comp, &temp);
                }
                if (ImGui::IsItemActivated())
                {
                    m_dragStartValue = *static_cast<std::string*>(data);
                }
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    std::string oldVal = std::any_cast<std::string>(m_dragStartValue);
                    std::string newVal = temp;

                    auto cmd = std::make_unique<ChangePropertyCommand<std::string>>(
                        comp, prop.m_setter, oldVal, newVal
                    );
                    HistoryManager::Instance().Do(std::move(cmd));
                }
            }
            // wchar_t
            else if (type == typeid(std::wstring))
            {
                ImGui::PushID(data);

                std::wstring temp = *static_cast<std::wstring*>(data);

                std::string strUtf8;
                if (!temp.empty())
                {
                    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0, NULL, NULL);
                    strUtf8.resize(size_needed);
                    WideCharToMultiByte(CP_UTF8, 0, &temp[0], (int)temp.size(), &strUtf8[0], size_needed, NULL, NULL);
                }

                char buffer[256];
                strncpy_s(buffer, strUtf8.c_str(), sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0';

                if (ImGui::InputText(name, buffer, sizeof(buffer)))
                {
                    std::string newUtf8 = buffer;
                    std::wstring wNew;
                    if (!newUtf8.empty())
                    {
                        int wsize_needed = MultiByteToWideChar(CP_UTF8, 0, &newUtf8[0], (int)newUtf8.size(), NULL, 0);
                        wNew.resize(wsize_needed);
                        MultiByteToWideChar(CP_UTF8, 0, &newUtf8[0], (int)newUtf8.size(), &wNew[0], wsize_needed);
                    }

                    temp = wNew;
                    prop.m_setter(comp, &temp);
                }

                if (ImGui::IsItemActivated())
                {
                    m_dragStartValue = *static_cast<std::wstring*>(data);
                }

                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    std::wstring oldVal = std::any_cast<std::wstring>(m_dragStartValue);

                    std::string newUtf8 = buffer;
                    std::wstring newVal;
                    if (!newUtf8.empty())
                    {
                        int wsize_needed = MultiByteToWideChar(CP_UTF8, 0, &newUtf8[0], (int)newUtf8.size(), NULL, 0);
                        newVal.resize(wsize_needed);
                        MultiByteToWideChar(CP_UTF8, 0, &newUtf8[0], (int)newUtf8.size(), &newVal[0], wsize_needed);
                    }

                    auto cmd = std::make_unique<ChangePropertyCommand<std::wstring>>(
                        comp, prop.m_setter, oldVal, newVal
                    );
                    HistoryManager::Instance().Do(std::move(cmd));
                }
                ImGui::PopID();
            }   
            // Vector3
            else if (type == typeid(Vector2))
            {
                Vector2 temp = *static_cast<Vector2*>(data);
                if (ImGui::DragFloat2(name, &temp.x, 0.1f))
                {
                    prop.m_setter(comp, &temp);
                }
                if (ImGui::IsItemActivated())
                {
                    m_dragStartValue = *static_cast<Vector2*>(data);
                }
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    Vector2 oldVal = std::any_cast<Vector2>(m_dragStartValue);
                    Vector2 newVal = temp;

                    auto cmd = std::make_unique<ChangePropertyCommand<Vector2>>(
                        comp, prop.m_setter, oldVal, newVal
                    );
                    HistoryManager::Instance().Do(std::move(cmd));
                }
            }

            // Vector3
            else if (type == typeid(Vector3))
            {
                Vector3 temp = *static_cast<Vector3*>(data);
                if (ImGui::DragFloat3(name, &temp.x, 0.1f))
                {
                    prop.m_setter(comp, &temp);
                }
                if (ImGui::IsItemActivated())
                {
                    m_dragStartValue = *static_cast<Vector3*>(data);
                }
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    Vector3 oldVal = std::any_cast<Vector3>(m_dragStartValue);
                    Vector3 newVal = temp;

                    auto cmd = std::make_unique<ChangePropertyCommand<Vector3>>(
                        comp, prop.m_setter, oldVal, newVal
                    );
                    HistoryManager::Instance().Do(std::move(cmd));
                }
            }
            // Vector4 (추가)
            else if (type == typeid(Vector4))
            {
                Vector4 temp = *static_cast<Vector4*>(data);
                if (ImGui::DragFloat4(name, &temp.x, 0.1f))
                {
                    prop.m_setter(comp, &temp);
                }
                if (ImGui::IsItemActivated())
                {
                    m_dragStartValue = *static_cast<Vector4*>(data);
                }
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    Vector4 oldVal = std::any_cast<Vector4>(m_dragStartValue);
                    Vector4 newVal = temp;

                    auto cmd = std::make_unique<ChangePropertyCommand<Vector4>>(
                        comp, prop.m_setter, oldVal, newVal
                    );
                    HistoryManager::Instance().Do(std::move(cmd));
                }
            }
            // Quaternion (m_rotation 특별 처리)
            else if (type == typeid(Quaternion))
            {
                Transform* tf = dynamic_cast<Transform*>(comp);

                if (tf && prop.m_name == "m_rotation")
                {
                    Vector3 displayEuler = tf->GetEditorEuler();
                    if (ImGui::DragFloat3("Rotation", &displayEuler.x, 0.5f))
                    {
                        tf->SetRotationEuler(displayEuler);
                    }
                    if (ImGui::IsItemActivated())
                    {
                        m_editorDragStartRotation = tf->GetEditorEuler();
                    }
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        auto cmd = std::make_unique<ChangeEditorTransformCommand>(
                            tf,
                            tf->GetPosition(), tf->GetPosition(),
                            m_editorDragStartRotation, tf->GetEditorEuler(),
                            tf->GetScale(), tf->GetScale()
                        );
                        HistoryManager::Instance().Do(std::move(cmd));
                    }
                }
                else // m_rotation이 아닌 다른 쿼터니언
                {
                    Quaternion temp = *static_cast<Quaternion*>(data);
                    if (ImGui::DragFloat4(name, &temp.x, 0.1f))
                    {
                        prop.m_setter(comp, &temp);
                    }
                    if (ImGui::IsItemActivated())
                    {
                        m_dragStartValue = *static_cast<Quaternion*>(data);
                    }
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        Quaternion oldVal = std::any_cast<Quaternion>(m_dragStartValue);
                        Quaternion newVal = temp;

                        auto cmd = std::make_unique<ChangePropertyCommand<Quaternion>>(
                            comp, prop.m_setter, oldVal, newVal
                        );
                        HistoryManager::Instance().Do(std::move(cmd));
                    }
                }
            }
            // Transform* (포인터)
            //else if (type == typeid(Transform*))
            //{
            //    //Transform* parentTf = *static_cast<Transform**>(data);
            //    //std::string parentName = (parentTf) ? parentTf->_GetOwner()->GetName() : "None (Root)";
            //    //ImGui::Text("%s: %s", name, parentName.c_str());
            //}


            // GameObject*
            else if (type == typeid(GameObject*))
            {
                GameObject* currentGO = *static_cast<GameObject**>(data);

                // 1. 현재 할당된 오브젝트 이름 표시 버튼
                std::string displayStr = "None";
                if (currentGO) displayStr = currentGO->GetName();

                float width = ImGui::CalcItemWidth();
                if (ImGui::Button(displayStr.c_str(), ImVec2(width, 0)))
                {
                    // 버튼 클릭 시 해당 오브젝트를 하이어라키에서 선택
                    if (currentGO) m_selectedGameObject = currentGO;
                }

                // 2. 드래그 앤 드롭 처리 (Hierarchy Item + Project File)
                if (ImGui::BeginDragDropTarget())
                {
                    // [기존] 하이어라키에서 드래그했을 때
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DRAG_ITEM"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(Transform*));
                        Transform* draggedTf = *(Transform**)payload->Data;
                        GameObject* draggedGO = draggedTf ? draggedTf->_GetOwner() : nullptr;

                        if (draggedGO)
                        {
                            GameObject* oldVal = currentGO;
                            prop.m_setter(comp, &draggedGO);

                            auto cmd = std::make_unique<ChangePropertyCommand<GameObject*>>(
                                comp, prop.m_setter, oldVal, draggedGO
                            );
                            HistoryManager::Instance().Do(std::move(cmd));
                        }
                    }

                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_FILE"))
                    {
                        const char* droppedPath = (const char*)payload->Data;
                        std::filesystem::path fpath(droppedPath);
                        std::string ext = fpath.extension().string();

                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                        if (ext == ".prefab")
                        {
							Prefab* prefabAsset = ResourceManager::Instance().Load<Prefab>(droppedPath);

                            GameObject* newInstance = prefabAsset->Instantiate();

                            if (newInstance)
                            {
                                newInstance->SetActive(false); 
								newInstance->SetFlag(GameObject::Flags::HideAndDontSave, true);

                                GameObject* oldVal = currentGO;
                                prop.m_setter(comp, &newInstance);

                                auto cmd = std::make_unique<ChangePropertyCommand<GameObject*>>(
                                    comp, prop.m_setter, oldVal, newInstance
                                );
                                HistoryManager::Instance().Do(std::move(cmd));

                                std::cout << "[Editor] Prefab assigned: " << newInstance->GetName() << std::endl;
                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
                ImGui::Text("%s", name);
            }


            // component 인 경우 
            else if (ReflectionDatabase::Instance().IsComponentPointer(type))
            {
                Component* currentComp = *static_cast<Component**>(data);

                std::string displayStr = "None";
                if (currentComp)
                {
                    displayStr = currentComp->_GetTypeName();
                    if (currentComp->_GetOwner())
                        displayStr += " (" + currentComp->_GetOwner()->GetName() + ")";
                }

                float width = ImGui::CalcItemWidth();
                if (ImGui::Button(displayStr.c_str(), ImVec2(width, 0)))
                {
                    if (currentComp && currentComp->_GetOwner()) {
                        m_selectedGameObject = currentComp->_GetOwner();
                    }
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DRAG_ITEM"))
                    {
                        Transform* draggedTf = *(Transform**)payload->Data;
                        GameObject* draggedGO = draggedTf ? draggedTf->_GetOwner() : nullptr;

                        if (draggedGO && prop.m_componentFinder)
                        {
                            void* targetComponent = prop.m_componentFinder(draggedGO);

                            if (targetComponent)
                            {
                                Component* oldVal = currentComp;
                                Component* newVal = static_cast<Component*>(targetComponent);

                                prop.m_setter(comp, &newVal);

                                auto cmd = std::make_unique<ChangePropertyCommand<Component*>>(
                                    comp, prop.m_setter, oldVal, newVal
                                );
                                HistoryManager::Instance().Do(std::move(cmd));
                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::SameLine();
                ImGui::Text("%s", prop.m_name.c_str());
            }

                // Texture* (Asset)
            else if (type == typeid(Texture*))
            {
                Texture* currentTex = *static_cast<Texture**>(data);

                std::vector<std::string> exts = { ".png", ".jpg", ".dds", ".tga", ".bmp" };
                DrawAssetReference<Texture>(this, name, currentTex, comp, prop.m_setter, exts);
            }
               
            //else if (
            // 
            // typeid(Material*))
            //{
            //    Material* currentMat = *static_cast<Material**>(data);
            //    std::vector<std::string> exts = { ".mat" };
            //    DrawAssetReference<Material>(this, name, currentMat, comp, prop.m_setter, exts);
            //}

            else if (type == typeid(TilemapData*)) {
				TilemapData* currentData = *static_cast<TilemapData**>(data);

				std::vector<std::string> exts = { ".tilemap" };
				DrawAssetReference<TilemapData>(this, name, currentData, comp, prop.m_setter, exts);
            }

			else if (type == typeid(Prefab*)) {
				Prefab* currentPrefab = *static_cast<Prefab**>(data);
				std::vector<std::string> exts = { ".prefab" };
                DrawAssetReference<Prefab>(this, name, currentPrefab, comp, prop.m_setter, exts);
			}
            

            else {
                std::cout << "type not Registered: " << type.name() << std::endl;
            }
			ImGui::PopID();
        }

        

        if (MeshRenderer* renderer = dynamic_cast<MeshRenderer*>(comp))
        {
            Material* currentMat = renderer->GetSharedMaterial();

            if (currentMat)
            {
                ImGui::Spacing();
                ImGui::Separator();

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); 
                bool nodeOpen = ImGui::TreeNodeEx("Material Instance Properties", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed);
                ImGui::PopStyleColor();

                if (nodeOpen)
                {
                    //else
                    //{
                    //    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "[Status: Shared (Asset)]");
                    //}

                    if (renderer->IsMaterialInstanced())
                    {
                        //ImGui::AlignTextToFramePadding();

                        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "[Status: Instanced (Unique)]");

                        ImGui::SameLine();

                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                        if (ImGui::Button("Revert to Original Asset", ImVec2(200, 0)))
                        {
                            renderer->SetMaterialID(renderer->GetMaterialID());

                            ImGui::PopStyleColor(); 
                            ImGui::TreePop();       
                            ImGui::PopID();         
                            return;                 
                        }
                        ImGui::PopStyleColor();

                        ImGui::Spacing();
                    }
                    else
                    {
                        ImGui::AlignTextToFramePadding();

                        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "[Status: Shared (Asset)]");
                    }

                    ImGui::Separator();
                    ImGui::Spacing();

                    //ImGui::TextDisabled("(Modifying this creates a unique instance)");
                    ImGui::Spacing();

                    Vector4 color = currentMat->GetColor();

                    if (ImGui::ColorEdit4("##MaterialColor", &color.x))
                    {
                        renderer->GetMaterial()->SetColor(color);
                    }

                    ImGui::SameLine();

                    ImGui::Text("Base Color");

                    if (ImGui::IsItemActivated())
                    {
                        m_dragStartValue = currentMat->GetColor();
                    }
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        Vector4 oldVal = std::any_cast<Vector4>(m_dragStartValue);
                        Vector4 newVal = color;

                        auto setter = [](void* targetObj, void* val) {
                            MeshRenderer* mr = static_cast<MeshRenderer*>(static_cast<Component*>(targetObj));
                            Vector4* v = static_cast<Vector4*>(val);
                            mr->GetMaterial()->SetColor(*v);
                            };

                        auto cmd = std::make_unique<ChangePropertyCommand<Vector4>>(
                            renderer, setter, oldVal, newVal
                        );
                        HistoryManager::Instance().Do(std::move(cmd));
                    }

                    ImGui::Spacing();


                    ImGui::Text("UV Settings");

                    // 1. Tiling UI
                    Vector2 tiling = currentMat->GetTiling();
                    if (ImGui::DragFloat2("Tiling", &tiling.x, 0.05f))
                    {
                        renderer->GetMaterial()->SetTiling(tiling.x, tiling.y);
                    }

                    // Tiling Undo/Redo
                    if (ImGui::IsItemActivated()) m_dragStartValue = currentMat->GetTiling();
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        Vector2 oldVal = std::any_cast<Vector2>(m_dragStartValue);
                        Vector2 newVal = tiling;

                        auto setter = [](void* targetObj, void* val) {
                            MeshRenderer* mr = static_cast<MeshRenderer*>(static_cast<Component*>(targetObj));
                            Vector2* v = static_cast<Vector2*>(val);
                            mr->GetMaterial()->SetTiling(v->x, v->y);
                            };

                        auto cmd = std::make_unique<ChangePropertyCommand<Vector2>>(
                            renderer, setter, oldVal, newVal
                        );
                        HistoryManager::Instance().Do(std::move(cmd));
                    }

                    // 2. Offset UI
                    Vector2 offset = currentMat->GetOffset();
                    if (ImGui::DragFloat2("Offset", &offset.x, 0.05f))
                    {
                        renderer->GetMaterial()->SetOffset(offset.x, offset.y);
                    }

                    // Offset Undo/Redo
                    if (ImGui::IsItemActivated()) m_dragStartValue = currentMat->GetOffset();
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        Vector2 oldVal = std::any_cast<Vector2>(m_dragStartValue);
                        Vector2 newVal = offset;

                        auto setter = [](void* targetObj, void* val) {
                            MeshRenderer* mr = static_cast<MeshRenderer*>(static_cast<Component*>(targetObj));
                            Vector2* v = static_cast<Vector2*>(val);
                            mr->GetMaterial()->SetOffset(v->x, v->y);
                            };

                        auto cmd = std::make_unique<ChangePropertyCommand<Vector2>>(
                            renderer, setter, oldVal, newVal
                        );
                        HistoryManager::Instance().Do(std::move(cmd));
                    }

                    const char* modes[] = { "Opaque", "Transparent" };
                    int currentMode = (int)renderer->GetSharedMaterial()->GetRenderMode();

                    if (ImGui::Combo("Render Mode", &currentMode, modes, IM_ARRAYSIZE(modes)))
                    {
                        renderer->GetMaterial()->SetRenderMode((RenderMode)currentMode);
                    }
                    const char* cullModes[] = { "Back", "Front", "None" };
                    int currentCull = (int)renderer->GetSharedMaterial()->GetCullMode();

                    if (ImGui::Combo("Cull Mode", &currentCull, cullModes, IM_ARRAYSIZE(cullModes)))
                    {
                        renderer->GetMaterial()->SetCullMode((CullMode)currentCull);
                    }

                    if (ImGui::IsItemActivated()) m_dragStartValue = (int)renderer->GetMaterial()->GetCullMode();
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        int oldVal = std::any_cast<int>(m_dragStartValue);
                        int newVal = currentCull;

                        auto setter = [](void* targetObj, void* val) {
                            MeshRenderer* mr = static_cast<MeshRenderer*>(static_cast<Component*>(targetObj));
                            int* v = static_cast<int*>(val);
                            // int를 CullMode로 캐스팅하여 설정
                            mr->GetMaterial()->SetCullMode((CullMode)*v);
                            };

                        auto cmd = std::make_unique<ChangePropertyCommand<int>>(
                            renderer, setter, oldVal, newVal
                        );
                        HistoryManager::Instance().Do(std::move(cmd));
                    }

                    if (ImGui::IsItemActivated()) m_dragStartValue = (int)renderer->GetMaterial()->GetRenderMode();
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        int oldVal = std::any_cast<int>(m_dragStartValue);
                        int newVal = currentMode;

                        auto setter = [](void* targetObj, void* val) {
                            MeshRenderer* mr = static_cast<MeshRenderer*>(static_cast<Component*>(targetObj));
                            int* v = static_cast<int*>(val);
                            mr->GetMaterial()->SetRenderMode((RenderMode)*v);
                            };

                        auto cmd = std::make_unique<ChangePropertyCommand<int>>(
                            renderer, setter, oldVal, newVal
                        );
                        HistoryManager::Instance().Do(std::move(cmd));
                    }

                    ImGui::Spacing();
                    ImGui::Separator();



                    if (ImGui::TreeNodeEx("Textures", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        if (ImGui::BeginTable("TextureSlotTable", 3, ImGuiTableFlags_SizingStretchProp))
                        {
                            ImGui::TableSetupColumn("Texture", ImGuiTableColumnFlags_WidthStretch , 100.f); // 초기값 안넣어주면 깜빡거림
                            ImGui::TableSetupColumn("Close", ImGuiTableColumnFlags_WidthFixed, 30.0f);
                            ImGui::TableSetupColumn("SlotLabel", ImGuiTableColumnFlags_WidthFixed, 100.0f);

                            for (int i = 0; i < Material::MAX_TEXTURE_SLOTS; ++i)
                            {
                                ImGui::PushID(i);

                                Texture* tex = currentMat->GetTexture(i);
                                std::string texName = "None";
                                if (tex)
                                {
                                    uint64_t texID = tex->GetMeta().guid;
                                    std::string path = AssetDatabase::Instance().GetPathFromID(texID);
                                    if (!path.empty())
                                        texName = std::filesystem::path(path).filename().string();
                                    else
                                        texName = "Texture Loaded";
                                }
                                // -------------------

                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();

                                if (ImGui::Button(texName.c_str(), ImVec2(-FLT_MIN, 0.0f)))
                                {
                                    // 텍스처 선택 로직
                                }

                                if (ImGui::BeginDragDropTarget())
                                {
                                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_FILE"))
                                    {
                                        const char* droppedPath = (const char*)payload->Data;
                                        std::string ext = std::filesystem::path(droppedPath).extension().string();
                                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                                        if (ext == ".png" || ext == ".jpg" || ext == ".dds" || ext == ".tga" || ext == ".bmp")
                                        {
                                            Texture* newTex = ResourceManager::Instance().Load<Texture>(droppedPath);
                                            if (newTex)
                                            {
                                                renderer->GetMaterial()->SetTexture(i, newTex);
                                            }
                                        }
                                    }
                                    ImGui::EndDragDropTarget();
                                }

                                ImGui::TableNextColumn();
                                if (ImGui::Button("X", ImVec2(-FLT_MIN, 0.0f)))
                                {
                                    Material* matToCheck = nullptr;
                                    if (renderer->IsMaterialInstanced())
                                        matToCheck = renderer->GetMaterial();
                                    else
                                        matToCheck = renderer->GetSharedMaterial();

                                    if (matToCheck && matToCheck->GetTexture(i) != nullptr)
                                    {
                                        renderer->GetMaterial()->SetTexture(i, nullptr);
                                    }
                                }

                                ImGui::TableNextColumn();

                                ImGui::AlignTextToFramePadding();

                                const char* slotLabel = nullptr;
                                switch (i)
                                {
                                case 0: slotLabel = "Albedo";    break; // t0
                                case 1: slotLabel = "Normal";    break; // t1
                                case 2: slotLabel = "Specular";  break; // t2
                                case 3: slotLabel = "Metallic";  break; // t3
                                case 4: slotLabel = "Roughness"; break; // t4
                                case 5: slotLabel = "AO";        break; // t5
                                //case 6: slotLabel = "IBL";       break; // t7
                                //case 7: slotLabel = "SphereMap";    break; // t7 ?? 감마는 뭐야
                                default: break;
                                }

                                if (slotLabel)
                                    ImGui::Text("%s", slotLabel);
                                else
                                    ImGui::Text("Slot %d", i);

                                ImGui::PopID();
                            }
                            ImGui::EndTable();
                        }
                        ImGui::TreePop();
                    }

                    ImGui::TreePop(); 
                }
            }
        }
    }

    ImGui::PopID();
}

void EditorUI::AlignWithView()
{
    if (!m_selectedGameObject || !m_sceneCamera) return;

    Transform* targetTf = m_selectedGameObject->GetTransform();
    Transform* camTf = m_sceneCamera->_GetOwner()->GetTransform();

    if (!targetTf || !camTf) return;

    Vector3 oldPos = targetTf->GetPosition();
    Quaternion oldRot = targetTf->GetRotationQuat();
    Vector3 oldScale = targetTf->GetScale();

    Vector3 newPos = camTf->GetPosition();
    Quaternion newRot = camTf->GetRotationQuat();
    Vector3 newScale = oldScale; 

    auto cmd = std::make_unique<ChangeTransformCommand>(
        targetTf,
        oldPos, newPos,
        oldRot, newRot,
        oldScale, newScale
    );

    HistoryManager::Instance().Do(std::move(cmd));
}

void EditorUI::OnDropFile(const std::string& rawPath)
{
    fs::path path(rawPath);
    std::string ext = path.extension().string();

    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".fbx" || ext == ".obj" || ext == ".x" || ext == ".glb")
    {

        GameObject* go = ResourceManager::Instance().LoadModel(path.string());
        if (go && m_sceneCamera)
        {
            Transform* camTf = m_sceneCamera->_GetOwner()->GetTransform();
            if (camTf)
            {
                float spawnDistance = 8.0f;
                Vector3 spawnPos = camTf->GetPosition() + (camTf->Forward() * spawnDistance);

                go->GetTransform()->SetPosition(spawnPos);
            }
        }
        //std::cout << "[Editor] Created Model from: " << path.string() << std::endl;
    }
    else if (ext == ".prefab")
    {
        // 프리팹 인스턴스화

		Prefab* prefab = ResourceManager::Instance().Load<Prefab>(path.string());

        GameObject* go = prefab->Instantiate();

        if (go)
        {
            m_selectedGameObject = go; 
            std::cout << "[Editor] Instantiated Prefab: " << path.string() << std::endl;
        }
    }
}

void EditorUI::DrawAssetInspector(const std::string& path)
{

    std::string newPath = path;
    std::replace(newPath.begin(), newPath.end(), '\\', '/');


    fs::path filePath(newPath);
    std::string ext = filePath.extension().string();

    if (ext == ".mat")
    {
        Material* material = ResourceManager::Instance().Load<Material>(path);

        if (material)
        {
            ImGui::Text("Material: %s", filePath.filename().string().c_str());
            //ImGui::Text("These settings are applied when launching the editor.");
            ImGui::Separator();

            Shader* currentShader = material->GetShader();
            std::string shaderName = "None (Missing)";

            if (currentShader)
            {
                uint64_t shaderID = currentShader->GetMeta().guid;
                std::string shaderPath = AssetDatabase::Instance().GetPathFromID(shaderID);
                if (!shaderPath.empty())
                {
                    shaderName = fs::path(shaderPath).filename().string();
                }
            }

            ImGui::Text("Shader:");
            ImGui::SameLine();


            if (ImGui::Button(shaderName.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                uint64_t id = currentShader->GetMeta().guid;
                std::string pathStr = AssetDatabase::Instance().GetPathFromID(id);

                if (!pathStr.empty())
                {
                    std::filesystem::path path(pathStr);

                    m_currentProjectDirectory = path.parent_path();

                    //m_selectedAssetPath = pathStr;
                }
            }


            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_FILE"))
                {
                    const char* droppedPath = (const char*)payload->Data;
                    fs::path droppedFilePath(droppedPath);
                    std::string ext = droppedFilePath.extension().string();

                    if (ext == ".hlsl" || ext == ".fx")
                    {
                        Shader* newShader = ResourceManager::Instance().Load<Shader>(droppedPath);
                        if (newShader)
                        {
                            material->SetShader(newShader);
                            material->SaveFile(path);
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Separator();

            Vector4 color = material->GetColor();
            if (ImGui::ColorEdit4("Material Color", &color.x))
            {
                material->SetColor(color); // 값 변경 및 상수 버퍼 업데이트 트리거
                material->SaveFile(path);  // 변경 사항 저장
            }
            ImGui::Separator();

            float tiling[2] = { material->GetTiling().x, material->GetTiling().y };
            if (ImGui::DragFloat2("Tiling", tiling, 0.1f))
            {
                material->SetTiling(tiling[0], tiling[1]);
                material->SaveFile(path); 
            }

            float offset[2] = { material->GetOffset().x, material->GetOffset().y };
            if (ImGui::DragFloat2("Offset", offset, 0.01f))
            {
                material->SetOffset(offset[0], offset[1]);
                material->SaveFile(path); 
            }

            const char* modes[] = { "Opaque", "Transparent" };
            int currentMode = (int)material->GetRenderMode();

            if (ImGui::Combo("Render Mode", &currentMode, modes, IM_ARRAYSIZE(modes)))
            {
                material->SetRenderMode((RenderMode)currentMode);
                material->SaveFile(path); 
            }

            const char* cullModes[] = { "Back", "Front", "None" };
            int currentCull = (int)material->GetCullMode();

            if (ImGui::Combo("Cull Mode", &currentCull, cullModes, IM_ARRAYSIZE(cullModes)))
            {
                material->SetCullMode((CullMode)currentCull);
                material->SaveFile(path); // 변경 즉시 파일 저장
            }

    //        for (int i = 0; i < Material::MAX_TEXTURE_SLOTS; ++i)
    //        {
    //            ImGui::PushID(i);

    //            ImGui::Text("Texture Slot %d", i);

				//ImGui::SameLine();

    //            Texture* currentTex = material->GetTexture(i);
    //            std::string texName = "Empty";

    //            if (currentTex)
    //            {
    //                uint64_t id = currentTex->GetMeta().guid;
    //                std::string pathStr = AssetDatabase::Instance().GetPathFromID(id);
    //                if (!pathStr.empty())
    //                {    

				//		texName = fs::path(pathStr).filename().string();

    //                    //std::filesystem::path path(pathStr);
    //                    //m_currentProjectDirectory = path.parent_path();
    //                    //m_selectedAssetPath = pathStr;
    //                }
    //                else {
    //                    texName = "Missing (Path Not Found)";
    //                }
    //            }

    //            if (ImGui::Button(texName.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - 30, 0)))
    //            {
    //                // 클릭 시 동작 (옵션)
    //            }

    //            if (ImGui::BeginDragDropTarget())
    //            {
    //                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_FILE"))
    //                {
    //                    const char* droppedPath = (const char*)payload->Data;
    //                    std::string droppedExt = fs::path(droppedPath).extension().string();
    //                    std::transform(droppedExt.begin(), droppedExt.end(), droppedExt.begin(), ::tolower);

    //                    if (droppedExt == ".png" || droppedExt == ".jpg" || droppedExt == ".dds" || droppedExt == ".tga" || droppedExt == ".bmp")
    //                    {
    //                        Texture* newTex = ResourceManager::Instance().Load<Texture>(droppedPath);
    //                        if (newTex)
    //                        {
    //                            material->SetTexture(i, newTex);
    //                            material->SaveFile(path);
    //                        }
    //                    }
    //                }
    //                ImGui::EndDragDropTarget();
    //            }

    //            ImGui::SameLine();

    //            if (ImGui::Button("X", ImVec2(22, 0)))
    //            {
    //                if (material->SetTexture(i, nullptr)) {
    //                    material->SaveFile(path);
    //                }
    //            }

            //    ImGui::PopID();
            //    ImGui::Spacing();
            //}

            // 텍스쳐는 그냥 머터리얼 인스턴스에서 넣어주기 이 값으로 로딩하면 게속 초기화되니까 그냥
        }
    }
    else if (ext == ".png" || ext == ".jpg" || ext == ".dds" || ext == ".tga" || ext == ".bmp")
    {
        Texture* tex = ResourceManager::Instance().Load<Texture>(path);
        if (tex)
        {
            ImGui::Text("Texture Settings");
            ImGui::Separator();

            bool changed = false;

            const char* filters[] = { "Point", "Bilinear", "Trilinear" };
            int f = (int)tex->GetFilterMode();
            if (ImGui::Combo("Filter Mode", &f, filters, 3))
            {
                tex->SetFilterMode((FilterMode)f);
                changed = true;
            }

            const char* wraps[] = { "Repeat", "Clamp" };
            int w = (int)tex->GetWrapMode();
            if (ImGui::Combo("Wrap Mode", &w, wraps, 2))
            {
                tex->SetWrapMode((WrapMode)w);
                changed = true;
            }

            if (changed)
            {
                tex->SaveImportSettings(path);
            }

            ImGui::Separator();
            //ImGui::Text("Preview");
        }
    }
    else if (ext == ".tilemap") // 1. 사용하는 타일맵 확장자로 변경하세요 (예: .tm, .json)
    {
        TilemapData* tilemap = ResourceManager::Instance().Load<TilemapData>(path);

        if (tilemap)
        {
            ImGui::Text("Tilemap Editor: %s", filePath.filename().string().c_str());
            ImGui::Separator();

            int width = tilemap->GetWidth();
            int height = tilemap->GetHeight();
            bool changed = false;

            if (ImGui::InputInt("Width", &width))
            {
                if (width > 0) { tilemap->SetWidth(width); changed = true; }
            }
            if (ImGui::InputInt("Height", &height))
            {
                if (height > 0) { tilemap->SetHeight(height); changed = true; }
            }

            ImGui::Separator();
            ImGui::Text("Tile Data Grid");

            if (ImGui::BeginTable("TilemapGrid", width > 0 ? width : 1, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX))
            {
                for (int y = 0; y < height; ++y)
                {
                    ImGui::TableNextRow();
                    for (int x = 0; x < width; ++x)
                    {
                        ImGui::TableNextColumn();

                        int tileID = tilemap->GetTileIndex(x, y);

                        ImGui::PushID(x + y * width);

                        std::string label = std::to_string(tileID);

                        int maxTileCount = TilemapGenerator::PALETTE_SIZE;

                        if (ImGui::Button(label.c_str(), ImVec2(30, 30)))
                        { 
                            tilemap->SetTileIndex(x, y, (tileID + 1) % maxTileCount);
                            changed = true;
                        }

                        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                        {

                            int nextID = tileID - 1;

                            if (nextID < 0)
                                nextID = maxTileCount - 1;

                            tilemap->SetTileIndex(x, y, nextID);
                            changed = true;
                        }
                        ImGui::PopID();
                    }
                }
                ImGui::EndTable();
            }

            if (changed)
            {
                tilemap->SaveFile(path);
            }

            ImGui::Separator();
        }
    }
    else
    {
        ImGui::Text("Selected Asset: %s", filePath.filename().string().c_str());
    }
}

void EditorUI::CreatePrimitive(const std::string& name, const std::string& assetPath)
{
    uint64_t modelID = AssetDatabase::Instance().GetIDFromPath(assetPath);

    if (modelID == 0)
    {
        std::cout << "[Editor] Error: Primitive asset not found at " << assetPath << std::endl;
        return;
    }

    auto scene = SceneManager::Instance().GetActiveScene();
    auto cmd = std::make_unique<CreateGameObjectCommand>(scene, name);
    CreateGameObjectCommand* pCmd = cmd.get();
    HistoryManager::Instance().Do(std::move(cmd));

    GameObject* newGO = pCmd->GetCreatedGameObject();

    MeshRenderer* mr = newGO->AddComponent<MeshRenderer>();

    mr->SetModelID(modelID);
    mr->SetMeshIndex(0);

    uint64_t defaultMatID = AssetDatabase::Instance().GetIDFromPath("Assets/Materials/Default.mat");
    if (defaultMatID != 0)
    {
        mr->SetMaterialID(defaultMatID);
    }

    m_selectedGameObject = newGO;
}


void EditorUI::RenderSceneWindow(RenderTexture* rt, Scene* activeScene , Camera* camera)
{
    m_sceneCamera = camera;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Scene");

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_sceneViewportSize = Vector2(viewportPanelSize.x, viewportPanelSize.y);

    if (rt->GetWidth() != (int)viewportPanelSize.x || rt->GetHeight() != (int)viewportPanelSize.y)
    {
        rt->Resize((int)viewportPanelSize.x, (int)viewportPanelSize.y);
    }

    if (camera)
    {
        float ratio = viewportPanelSize.x / viewportPanelSize.y;
        camera->SetAspectRatio(ratio);
    }

    ImGui::Image((void*)rt->GetSRV(), viewportPanelSize);

    bool isHovered = ImGui::IsWindowHovered();
    FreeCamera::SetIsSceneHovered(isHovered);

    ImVec2 imageMin = ImGui::GetItemRectMin();
    ImVec2 imageMax = ImGui::GetItemRectMax();

    // 피킹 테스트.
    // 이미지 위에서 좌클릭 했을 때만 + 기즈모 조작 중이 아닐 때만
    const bool hovered = ImGui::IsItemHovered();
    const bool clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    if (activeScene && camera && hovered && clicked && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
    {
        ImVec2 mouse = ImGui::GetMousePos();

        float localX = mouse.x - imageMin.x;
        float localY = mouse.y - imageMin.y;

        float viewW = imageMax.x - imageMin.x;
        float viewH = imageMax.y - imageMin.y;

        localX = std::clamp(localX, 0.0f, viewW);
        localY = std::clamp(localY, 0.0f, viewH);

        Ray ray = camera->ScreenPointToRay(localX, localY, viewW, viewH);

        GameObject* hit = nullptr;
        float hitT = 0.0f;

        if (activeScene->Raycast2(ray, hit, hitT))
        {
            //std::cout << hit->GetName() << std::endl;
            m_selectedGameObject = hit;
        }
        else
        {
            //std::cout << "일단 뭔가 되고는 있음" << std::endl;
            m_selectedGameObject = nullptr;
        }
    }
    // 


    ImGuizmo::SetRect(imageMin.x, imageMin.y, imageMax.x - imageMin.x, imageMax.y - imageMin.y);

    ImGuizmo::SetDrawlist();

    if (camera)
    {
        DrawGizmo(activeScene, camera);
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void EditorUI::RenderGameWindow(RenderTexture* rt, Scene* activeScene)  
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Game");

    ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 startPos = ImGui::GetCursorScreenPos(); 

    float refAspect = DX11Renderer::Instance().GetRefWidth() / static_cast<float>(DX11Renderer::Instance().GetRefHeight());
    float winAspect = size.x / size.y;
    float drawW = size.x, drawH = size.y;

    if (winAspect > refAspect) drawW = drawH * refAspect;
    else drawH = drawW / refAspect;

    float offX = (size.x - drawW) * 0.5f;
    float offY = (size.y - drawH) * 0.5f;

    m_gameViewportSize = Vector2(size.x, size.y);

    if (rt->GetWidth() != (int)drawW || rt->GetHeight() != (int)drawH)
    {
        rt->Resize((int)size.x, (int)size.y);
    }


	Scene* curScene = SceneManager::Instance().GetActiveScene();
    
	Camera* mainCamera = nullptr;

    if(curScene)
    {
        mainCamera = curScene->GetMainCamera();
	}

    if (mainCamera)
    {
        float ratio = drawW / drawH;
        mainCamera->SetAspectRatio(ratio);
    }

#ifdef _DEBUG
    InputManager::Instance().SetGameResolution((int)size.x, (int)size.y);
#endif
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offX, ImGui::GetCursorPosY() + offY));

    ImGui::Image((void*)rt->GetSRV(), ImVec2(drawW, drawH));


    bool isHovered = ImGui::IsItemHovered();

    bool isFocused = ImGui::IsWindowFocused();

    // 피킹 테스트.
    // 이미지 위에서 좌클릭 했을 때만 + 기즈모 조작 중이 아닐 때만
    const bool clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    if (activeScene && isHovered && clicked && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
    {
        Camera* camera = activeScene->GetMainCamera();
        if (camera) 
        {
            ImVec2 mouse = ImGui::GetMousePos();

            ImVec2 imgMin = ImGui::GetItemRectMin();
            ImVec2 imgMax = ImGui::GetItemRectMax();

            float localX = mouse.x - imgMin.x;
            float localY = mouse.y - imgMin.y;

            float viewW = imgMax.x - imgMin.x;
            float viewH = imgMax.y - imgMin.y;

            localX = std::clamp(localX, 0.0f, viewW);
            localY = std::clamp(localY, 0.0f, viewH);

            Ray ray = camera->ScreenPointToRay(localX, localY, viewW, viewH);

            GameObject* hit = nullptr;
            float hitT = 0.0f;

            if (activeScene->Raycast2(ray, hit, hitT))
            {
                std::cout << hit->GetName() << std::endl;
                m_selectedGameObject = hit;
            }
            else
            {
                //std::cout << "일단 뭔가 되고는 있음" << std::endl;
                m_selectedGameObject = nullptr;
            }
        }
    }
    // 

#ifdef _DEBUG
    if (isHovered) 
    {
        //InputManager::Instance().SetGameInputActive(true);

        ImVec2 mousePos = ImGui::GetMousePos();
        InputManager::Instance().SetEditorMousePos(
            (int)(mousePos.x - startPos.x),
            (int)(mousePos.y - startPos.y)
        );
    }
    else
#endif
    {
        //InputManager::Instance().SetGameInputActive(false);
    }

	//DX11Renderer::Instance().DrawString(10, 10, "Game Viewport", 1.0f, Vector4(1, 1, 1, 1));

    ImGui::End();
    ImGui::PopStyleVar();
}

void EditorUI::DrawProjectWindow(Game::EngineMode engineMode)
{
    ImGui::Begin("Project");

    if (m_currentProjectDirectory != "Assets")
    {
        if (ImGui::Button("<- Back"))
        {
            m_currentProjectDirectory = m_currentProjectDirectory.parent_path();
        }
        ImGui::SameLine();
    }
    ImGui::Text("Current Path: %s", m_currentProjectDirectory.string().c_str());
    ImGui::Separator();

    float padding = 16.0f;
    float thumbnailSize = 64.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    if (fs::exists(m_currentProjectDirectory))
    {
        for (auto& directoryEntry : fs::directory_iterator(m_currentProjectDirectory))
        {
            const auto& path = directoryEntry.path();
            std::string filename = path.filename().string();
			std::string ext = path.extension().string();

            std::string lowerCaseExt = ext;
            std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), ::tolower);

            if (ext == ".meta") continue;
			if (ext == ".cso") continue;
			if (ext == ".vso") continue;
            if (lowerCaseExt == ".svn") continue; 
            if (filename == ".svn") continue;
			if (lowerCaseExt == ".orig") continue; // git merge temp files
			if (filename == "PlayMode_Backup.scene") continue;

            ImGui::PushID(filename.c_str());

            bool isDirectory = directoryEntry.is_directory();

            Texture* iconToUse = isDirectory ? m_iconFolder : m_iconFile; 

            if (!isDirectory)
            {
                if (lowerCaseExt == ".fbx" || lowerCaseExt == ".obj" || lowerCaseExt == ".x" || lowerCaseExt == ".glb")
                {
                    iconToUse = m_iconModel ? m_iconModel : m_iconFile;
                }
                else if (lowerCaseExt == ".mat")
                {
                    iconToUse = m_iconMaterial ? m_iconMaterial : m_iconFile;
                }
                else if (lowerCaseExt == ".png" || lowerCaseExt == ".jpg" || lowerCaseExt == ".dds" || lowerCaseExt == ".tga" || lowerCaseExt == ".bmp")
                {
                    iconToUse = m_iconTexture ? m_iconTexture : m_iconFile;
                }
                else if (lowerCaseExt == ".mp3" || lowerCaseExt == ".wav" || lowerCaseExt == ".ogg")
                {
                    iconToUse = m_iconAudio ? m_iconAudio : m_iconFile;
                }
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

            if (iconToUse && iconToUse->GetSRV())
            {
                ImGui::ImageButton("##image", (ImTextureID)iconToUse->GetSRV(), ImVec2(thumbnailSize, thumbnailSize));
            }
            else
            {
                // 아이콘 로드 실패 시 텍스트 버튼으로 대체
                ImGui::Button(isDirectory ? "[Dir]" : "[File]", ImVec2(thumbnailSize, thumbnailSize));
            }

            if (ImGui::BeginPopupContextItem("##RenameContext"))
            {
                if (ImGui::MenuItem("Rename"))
                {
                    m_renameTargetFile = path.string();
                    strcpy_s(m_renameBuffer, filename.c_str());
                    m_showRenamePopup = true;
                }

                ImGui::EndPopup();
            }


            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (isDirectory)
                {
                    m_currentProjectDirectory /= path.filename();
                }
                else
                {
                    std::cout << "Selected File: " << filename << std::endl;

                    if (lowerCaseExt == ".scene")
                    {
                        if (engineMode == Game::EngineMode::Edit) {
                        
                            std::string sceneName = path.stem().string();

                            SceneManager::Instance().RegisterScene(path.string());

                            SceneManager::Instance().LoadScene(sceneName);

                        }

                        else {
                            std::cout << "플레이 중에 씬 전환 불가능" << std::endl;
                        }
                    }

                    m_selectedAssetPath = path.generic_string();
                    m_selectedGameObject = nullptr;
                }
            }
            ImGui::PopStyleColor();

            if (ImGui::BeginDragDropSource())
            {
                std::string itemPath = path.generic_string();
                ImGui::SetDragDropPayload("PROJECT_FILE", itemPath.c_str(), itemPath.size() + 1);
                ImGui::Text("%s", filename.c_str());
                ImGui::EndDragDropSource();
            }

            ImGui::TextWrapped("%s", filename.c_str());

            ImGui::NextColumn();
            ImGui::PopID();
        }
    }

    ImGui::Columns(1);


    ImVec2 avail = ImGui::GetContentRegionAvail();
    if (avail.y < 50.0f) avail.y = 50.0f; // 최소 높이 보장
    if (avail.x < 1.0f) avail.x = 1.0f;

    ImGui::InvisibleButton("##ProjectDropTarget", avail);

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DRAG_ITEM"))
        {
            IM_ASSERT(payload->DataSize == sizeof(Transform*));
            Transform* draggedTf = *(Transform**)payload->Data;
            GameObject* go = draggedTf ? draggedTf->_GetOwner() : nullptr;

            if (go)
            {
                std::string fileName = go->GetName() + ".prefab";
                std::filesystem::path fullPath = m_currentProjectDirectory / fileName;

                int counter = 1;
                while (std::filesystem::exists(fullPath))
                {
                    fileName = go->GetName() + "_" + std::to_string(counter) + ".prefab";
                    fullPath = m_currentProjectDirectory / fileName;
                    counter++;
                }

                if (ResourceManager::Instance().SavePrefab(go, fullPath.string()))
                {
                    std::cout << "[Editor] Prefab saved successfully: " << fullPath.string() << std::endl;

                    AssetDatabase::Instance().ProcessAssetFile(fullPath.string());
                }
                else
                {
                    std::cerr << "[Editor] Failed to save prefab." << std::endl;
                }
            }
        }

        ImGui::EndDragDropTarget();
    }


    if (ImGui::BeginPopupContextWindow("ProjectContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
    {
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::MenuItem("Material"))
            {
                std::string baseName = "New Material";
                std::string ext = ".mat";
                fs::path newPath = m_currentProjectDirectory / (baseName + ext);

                int counter = 1;
                while (fs::exists(newPath))
                {
                    newPath = m_currentProjectDirectory / (baseName + " " + std::to_string(counter) + ext);
                    counter++;
                }

                fs::path shaderPath = fs::path("Assets") / "Shaders" / "Default_VS.hlsl";
                uint64_t defaultShaderID = AssetDatabase::Instance().GetIDFromPath(shaderPath.string());

                if (defaultShaderID == 0)
                {
                    std::cout << "[Editor] Warning: Could not find default shader at " << shaderPath.string() << std::endl;
                }

                std::ofstream file(newPath);
                if (file.is_open())
                {
                    file << "{\n";
                    file << "    \"ShaderID\": " << defaultShaderID << ",\n";
                    file << "    \"Textures\": {}\n";
                    file << "}";
                    file.close();

                    std::cout << "[Editor] Created new material: " << newPath.string() << " (ShaderID: " << defaultShaderID << ")" << std::endl;

                }

                AssetDatabase::Instance().ProcessAssetFile(newPath.string());
            }

            if (ImGui::MenuItem("Scene"))
            {
                std::string baseName = "New Scene";
                std::string ext = ".scene";
                fs::path newPath = m_currentProjectDirectory / (baseName + ext);

                // 파일 이름 중복 방지
                int counter = 1;
                while (fs::exists(newPath))
                {
                    newPath = m_currentProjectDirectory / (baseName + " " + std::to_string(counter) + ext);
                    counter++;
                }

                uint64_t goID = IDManager::Instance().GetNewUniqueID();
                uint64_t tfID = IDManager::Instance().GetNewUniqueID();
                uint64_t camID = IDManager::Instance().GetNewUniqueID();

                std::ofstream file(newPath);
                if (file.is_open())
                {
                    file << "{\n";
                    file << "  \"gameObjects\": [\n";
                    file << "    {\n";
                    file << "      \"active\": true,\n";
                    file << "      \"components\": [\n";
                    file << "        {\n";
                    file << "          \"Active\": true,\n";
                    file << "          \"id\": " << camID << ",\n";
                    file << "          \"m_clearColor\": {\n";
                    file << "            \"w\": 0.20000000298023224,\n";
                    file << "            \"x\": 0.20000000298023224,\n";
                    file << "            \"y\": 0.20000000298023224,\n";
                    file << "            \"z\": 0.20000000298023224\n";
                    file << "          },\n";
                    file << "          \"m_editorFovY\": 60.0,\n";
                    file << "          \"m_farZ\": 1000.0,\n";
                    file << "          \"m_isOrthographic\": false,\n";
                    file << "          \"m_nearZ\": 0.009999999776482582,\n";
                    file << "          \"m_orthographicSize\": 5.0,\n";
                    file << "          \"m_viewportRect\": {\n";
                    file << "            \"w\": 1.0,\n";
                    file << "            \"x\": 0.0,\n";
                    file << "            \"y\": 0.0,\n";
                    file << "            \"z\": 1.0\n";
                    file << "          },\n";
                    file << "          \"typeName\": \"Camera\"\n";
                    file << "        }\n";
                    file << "      ],\n";
                    file << "      \"id\": " << goID << ",\n";
                    file << "      \"name\": \"MainCamera\",\n";
                    file << "      \"tag\": \"Untagged\",\n";
                    file << "      \"transform\": {\n";
                    file << "        \"id\": " << tfID << ",\n";
                    file << "        \"m_editorEulerAngles\": {\n";
                    file << "          \"x\": 0.0,\n";
                    file << "          \"y\": 0.0,\n";
                    file << "          \"z\": 0.0\n";
                    file << "        },\n";
                    file << "        \"m_parent\": 0,\n";
                    file << "        \"m_position\": {\n";
                    file << "          \"x\": 0.0,\n";
                    file << "          \"y\": 0.0,\n";
                    file << "          \"z\": 0.0\n";
                    file << "        },\n";
                    file << "        \"m_rotation\": {\n";
                    file << "          \"w\": 1.0,\n";
                    file << "          \"x\": 0.0,\n";
                    file << "          \"y\": 0.0,\n";
                    file << "          \"z\": 0.0\n";
                    file << "        },\n";
                    file << "        \"m_scale\": {\n";
                    file << "          \"x\": 1.0,\n";
                    file << "          \"y\": 1.0,\n";
                    file << "          \"z\": 1.0\n";
                    file << "        }\n";
                    file << "      }\n";
                    file << "    }\n";
                    file << "  ]\n";
                    file << "}";
                    file.close();

                    AssetDatabase::Instance().ProcessAssetFile(newPath.string());
                    std::cout << "[Editor] Created new scene with JSON template: " << newPath.string() << std::endl;
                }
                else
                {
                    std::cerr << "[Editor] Failed to write scene file." << std::endl;
                }
            }

            if (ImGui::MenuItem("Tilemap"))
            {
                std::string baseName = "New Tilemap";
                std::string ext = ".tilemap";
                std::filesystem::path newPath = m_currentProjectDirectory / (baseName + ext);

                int counter = 1;
                while (std::filesystem::exists(newPath))
                {
                    newPath = m_currentProjectDirectory / (baseName + " " + std::to_string(counter) + ext);
                    counter++;
                }

                std::ofstream file(newPath);
                if (file.is_open())
                {
                    file << "{\n";
                    file << "  \"width\": 10,\n";
                    file << "  \"height\": 10,\n";
                    file << "  \"p0\": 0, \"p1\": 0, \"p2\": 0, \"p3\": 0, \"p4\": 0,\n";

                    file << "  \"grid\": [\n";
                    for (int i = 0; i < 100; ++i)
                    {
                        file << "    { \"v\": -1 }" << (i < 99 ? "," : "") << "\n";
                    }
                    file << "  ]\n";
                    file << "}";

                    file.close();

                    AssetDatabase::Instance().ProcessAssetFile(newPath.string());

                    std::cout << "[Editor] Created new tilemap: " << newPath.string() << std::endl;
                }
                else
                {
                    std::cerr << "[Editor] Failed to create tilemap file." << std::endl;
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }

    if (m_showRenamePopup)
    {
        ImGui::OpenPopup("Rename Asset");
        m_showRenamePopup = false;
    }

    if (ImGui::BeginPopupModal("Rename Asset", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter new name:");

        if (ImGui::IsWindowAppearing())
            ImGui::SetKeyboardFocusHere();

        bool enterPressed = ImGui::InputText("##rename", m_renameBuffer, sizeof(m_renameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);

        if (ImGui::Button("OK") || enterPressed)
        {
            std::string newName = m_renameBuffer;
            if (!newName.empty())
            {

                std::filesystem::path oldPath(m_renameTargetFile);
                std::string ext = oldPath.extension().string(); 

                // 입력된 새 이름에 확장자가 없거나 다르면 원래 확장자를 붙여줌
                if (std::filesystem::path(newName).extension().string() != ext)
                {
                    newName += ext;
                }

                if (AssetDatabase::Instance().RenameAsset(m_renameTargetFile, newName))
                {
                    std::filesystem::path pOld(m_renameTargetFile);
                    std::filesystem::path pNew = pOld.parent_path() / newName;

                    ResourceManager::Instance().MoveResource(m_renameTargetFile, pNew.string());
                }
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}


void EditorUI::DrawOverlay()
{
    static float elapsed = 0.0f;
    static float displayedFPS = 60.0f; 
    static float displayedMS = 16.6f;  

    elapsed += ImGui::GetIO().DeltaTime;

    if (elapsed > 1.0f)
    {
        displayedFPS = ImGui::GetIO().Framerate;
        displayedMS = 1000.0f / (displayedFPS > 0.0f ? displayedFPS : 1.0f);
        elapsed = 0.0f;
    }


    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    const float PAD = 10.0f;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 workPos = viewport->WorkPos;

    // 텍스트 내용 만들기
    char overlayText[64];
    sprintf_s(overlayText, "FPS: %.1f\n%.2f ms", displayedFPS, displayedMS);

    ImVec2 windowPos = ImVec2(workPos.x + PAD, workPos.y + PAD + 30.0f);

    ImVec2 textSize = ImGui::CalcTextSize(overlayText);
    ImVec2 padding = ImVec2(10.0f, 5.0f);
    ImVec2 boxMin = windowPos;
    ImVec2 boxMax = ImVec2(windowPos.x + textSize.x + padding.x * 2, windowPos.y + textSize.y + padding.y * 2);

    drawList->AddRectFilled(boxMin, boxMax, IM_COL32(0, 0, 0, 150), 5.0f); 

    drawList->AddText(ImVec2(windowPos.x + padding.x, windowPos.y + padding.y),
        IM_COL32(255, 255, 0, 255), overlayText);
}
