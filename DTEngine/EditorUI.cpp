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
#include "Camera.h"

namespace fs = std::filesystem;

static ImGuizmo::OPERATION m_currentOperation = ImGuizmo::TRANSLATE;
static ImGuizmo::MODE      m_currentMode      = ImGuizmo::LOCAL;

EditorUI::EditorUI() = default;
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

        bool isPlay = (currentMode == Game::EngineMode::Play);
        if (isPlay) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.7f, 1.0f, 1.0f));

        if (ImGui::Button(isPlay ? "Stop" : "Play", ImVec2(buttonWidth, 0)))
        {
            onModeChanged(Game::EngineMode::Play);
        }

        if (isPlay) ImGui::PopStyleColor();

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

void EditorUI::Render(Scene* activeScene)
{
    if (!activeScene) return;
    DrawHierarchyWindow(activeScene);
    DrawInspectorWindow();
    DrawProjectWindow();
    //DrawGizmo(activeScene);
    
    DrawOverlay();

    // Redo Undo

    bool ctrlPressed = InputManager::Instance().GetKey(KeyCode::Control);
    bool shiftPressed = InputManager::Instance().GetKey(KeyCode::Shift);
    bool zPressed_Down = InputManager::Instance().GetKeyDown(KeyCode::Z);
    bool yPressed_Down = InputManager::Instance().GetKeyDown(KeyCode::Y);

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

            ImGuizmo::Manipulate(
                (float*)&viewMatrix,
                (float*)&projMatrix,
                m_currentOperation,
                m_currentMode,
                (float*)&worldMatrix
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

    if (m_selectedGameObject)
    {
        bool oldState = m_selectedGameObject->IsActive();
        bool newState = oldState;

        if (ImGui::Checkbox("##ActiveCheckbox", &newState))
        {
            auto cmd = std::make_unique<ChangeGameObjectActiveCommand>(m_selectedGameObject, oldState, newState);
            HistoryManager::Instance().Do(std::move(cmd));
        }

        ImGui::SameLine();

        char nameBuffer[128];
        strncpy_s(nameBuffer, m_selectedGameObject->GetName().c_str(), sizeof(nameBuffer) - 1);
        nameBuffer[sizeof(nameBuffer) - 1] = '\0';

        ImGui::SetNextItemWidth(150);
        if (ImGui::InputText("##NameInput", nameBuffer, sizeof(nameBuffer)))
        {
            m_selectedGameObject->SetName(nameBuffer);
        }

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();

        char tagBuffer[128];
        strncpy_s(tagBuffer, m_selectedGameObject->GetTag().c_str(), sizeof(tagBuffer) - 1);
        tagBuffer[sizeof(tagBuffer) - 1] = '\0';

        ImGui::SetNextItemWidth(80);
        if (ImGui::InputText("Tag", tagBuffer, sizeof(tagBuffer)))
        {
            m_selectedGameObject->SetTag(tagBuffer);
        }

        ImGui::Separator();

        DrawComponentProperties(m_selectedGameObject->GetTransform());

        for (const auto& comp : m_selectedGameObject->_GetComponents())
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
                    auto cmd = std::make_unique<AddComponentCommand>(m_selectedGameObject, typeName);
                    HistoryManager::Instance().Do(std::move(cmd));
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
    }
    else if (!m_selectedAssetPath.empty())
    {
        DrawAssetInspector(m_selectedAssetPath);
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
        ImGui::EndPopup();
    }


    if (header_open)
    {
        for (const PropertyInfo& prop : info->m_properties)
        {
            if (prop.m_name == "m_editorEulerAngles")
            {
                continue;
            }

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
            if (!prop.m_enumNames.empty() && type == typeid(int))
            {
                int* valPtr = static_cast<int*>(data);
                int currentItem = *valPtr;

                std::vector<const char*> items;
                for (const auto& enumName : prop.m_enumNames)
                    items.push_back(enumName.c_str());

                if (ImGui::Combo(name, &currentItem, items.data(), (int)items.size()))
                {
                    prop.m_setter(comp, &currentItem);
                }

                if (ImGui::IsItemActivated()) m_dragStartValue = *valPtr;
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    int oldVal = std::any_cast<int>(m_dragStartValue);
                    auto cmd = std::make_unique<ChangePropertyCommand<int>>(comp, prop.m_setter, oldVal, currentItem);
                    HistoryManager::Instance().Do(std::move(cmd));
                }
            }
            // float
            else if (type == typeid(float))
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
                            if (ext == ".fbx" || ext == ".obj" || ext == ".x" || ext == ".blend") isFormatValid = true;
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
            else if (type == typeid(Transform*))
            {
                //Transform* parentTf = *static_cast<Transform**>(data);
                //std::string parentName = (parentTf) ? parentTf->_GetOwner()->GetName() : "None (Root)";
                //ImGui::Text("%s: %s", name, parentName.c_str());
            }
            // GameObject*
            else if (type == typeid(GameObject*))
            {
                GameObject* go = *static_cast<GameObject**>(data);
                std::string goName = (go) ? go->GetName() : "None";
                ImGui::Text("%s: %s", name, goName.c_str());
            }
            else if (type == typeid(Camera*))
            {
                Camera* cam = *static_cast<Camera**>(data);

                std::string displayStr = "None (Camera)";
                if (cam && cam->_GetOwner())
                {
                    displayStr = cam->_GetOwner()->GetName();
                }

                float width = ImGui::CalcItemWidth();

                if (ImGui::Button(displayStr.c_str(), ImVec2(width, 0)))
                {
                    if (cam && cam->_GetOwner())
                    {
                        m_selectedGameObject = cam->_GetOwner(); 
                    }
                }

                // 드래그 앤 드롭 타겟 설정
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DRAG_ITEM"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(Transform*));
                        Transform* draggedTf = *(Transform**)payload->Data;

                        if (draggedTf)
                        {
                            Camera* newCam = draggedTf->_GetOwner()->GetComponent<Camera>();

                            if (newCam)
                            {
                                Camera* oldVal = cam;

                                prop.m_setter(comp, &newCam);

                                auto cmd = std::make_unique<ChangePropertyCommand<Camera*>>(
                                    comp, prop.m_setter, oldVal, newCam
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

            // Texture*
            else if (type == typeid(Texture*))
            {
                // 데이터 포인터를 Texture**로 캐스팅하여 현재 값 가져오기
                Texture** texPtr = static_cast<Texture**>(data);
                Texture* currentTex = *texPtr;

                // 텍스처 이름 또는 "None" 표시용 문자열 생성
                std::string displayStr = "None";
                if (currentTex)
                {
                    uint64_t id = currentTex->GetMeta().guid;
                    std::string path = AssetDatabase::Instance().GetPathFromID(id);
                    if (!path.empty())
                        displayStr = std::filesystem::path(path).filename().string();
                    else
                        displayStr = "Loaded Texture";
                }

                float width = ImGui::CalcItemWidth();

                if (ImGui::Button(displayStr.c_str(), ImVec2(width, 0)))
                {
                    if (currentTex)
                    {
                        uint64_t id = currentTex->GetMeta().guid;
                        std::string pathStr = AssetDatabase::Instance().GetPathFromID(id);
                        if (!pathStr.empty())
                        {
                            m_currentProjectDirectory = std::filesystem::path(pathStr).parent_path();
                        }
                    }
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_FILE"))
                    {
                        const char* droppedPath = (const char*)payload->Data;
                        std::string ext = std::filesystem::path(droppedPath).extension().string();
                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                        if (ext == ".png" || ext == ".jpg" || ext == ".dds" || ext == ".tga")
                        {
                            Texture* newTex = ResourceManager::Instance().Load<Texture>(droppedPath);
                            if (newTex)
                            {
                                Texture* oldVal = currentTex;

                                prop.m_setter(comp, &newTex);

                                auto cmd = std::make_unique<ChangePropertyCommand<Texture*>>(
                                    comp, prop.m_setter, oldVal, newTex
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

            

            else {
                std::cout << "type not Registered: " << type.name() << std::endl;
            }
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
                    int currentMode = (int)renderer->GetMaterial()->GetRenderMode();

                    if (ImGui::Combo("Render Mode", &currentMode, modes, IM_ARRAYSIZE(modes)))
                    {
                        renderer->GetMaterial()->SetRenderMode((RenderMode)currentMode);
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
                            ImGui::TableSetupColumn("Close", ImGuiTableColumnFlags_WidthFixed, 25.0f);
                            ImGui::TableSetupColumn("SlotLabel", ImGuiTableColumnFlags_WidthFixed, 60.0f);

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

    if (ext == ".fbx" || ext == ".obj" || ext == ".x")
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
}

void EditorUI::DrawAssetInspector(const std::string& path)
{
    fs::path filePath(path);
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

            for (int i = 0; i < Material::MAX_TEXTURE_SLOTS; ++i)
            {
                ImGui::PushID(i);

                ImGui::Text("Texture Slot %d", i);

				ImGui::SameLine();

                Texture* currentTex = material->GetTexture(i);
                std::string texName = "Empty";

                if (currentTex)
                {
                    uint64_t id = currentTex->GetMeta().guid;
                    std::string pathStr = AssetDatabase::Instance().GetPathFromID(id);
                    if (!pathStr.empty())
                    {    

						texName = fs::path(pathStr).filename().string();

                        //std::filesystem::path path(pathStr);
                        //m_currentProjectDirectory = path.parent_path();
                        //m_selectedAssetPath = pathStr;
                    }
                    else {
                        texName = "Missing (Path Not Found)";
                    }
                }

                if (ImGui::Button(texName.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - 30, 0)))
                {
                    // 클릭 시 동작 (옵션)
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_FILE"))
                    {
                        const char* droppedPath = (const char*)payload->Data;
                        std::string droppedExt = fs::path(droppedPath).extension().string();
                        std::transform(droppedExt.begin(), droppedExt.end(), droppedExt.begin(), ::tolower);

                        if (droppedExt == ".png" || droppedExt == ".jpg" || droppedExt == ".dds" || droppedExt == ".tga" || droppedExt == ".bmp")
                        {
                            Texture* newTex = ResourceManager::Instance().Load<Texture>(droppedPath);
                            if (newTex)
                            {
                                material->SetTexture(i, newTex);
                                material->SaveFile(path);
                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();

                if (ImGui::Button("X", ImVec2(22, 0)))
                {
                    if (material->SetTexture(i, nullptr)) {
                        material->SaveFile(path);
                    }
                }

                ImGui::PopID();
                ImGui::Spacing();
            }
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

    if (rt->GetWidth() != (int)viewportPanelSize.x || rt->GetHeight() != (int)viewportPanelSize.y)
    {
        rt->Resize((int)viewportPanelSize.x, (int)viewportPanelSize.y);
    }

    ImGui::Image((void*)rt->GetSRV(), viewportPanelSize);

    bool isHovered = ImGui::IsWindowHovered();
    FreeCamera::SetIsSceneHovered(isHovered);

    ImVec2 imageMin = ImGui::GetItemRectMin();
    ImVec2 imageMax = ImGui::GetItemRectMax();

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

    if (rt->GetWidth() != (int)size.x || rt->GetHeight() != (int)size.y)
    {
        rt->Resize((int)size.x, (int)size.y);
    }

    ImGui::Image((void*)rt->GetSRV(), size);

	//DX11Renderer::Instance().DrawString(10, 10, "Game Viewport", 1.0f, Vector4(1, 1, 1, 1));

    ImGui::End();
    ImGui::PopStyleVar();
}

void EditorUI::DrawProjectWindow()
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

            if (ext == ".meta") continue;
			if (ext == ".cso") continue;
			if (ext == ".vso") continue;
			if (filename == "PlayMode_Backup.scene") continue;

            ImGui::PushID(filename.c_str());

            bool isDirectory = directoryEntry.is_directory();

            if (isDirectory)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.7f, 0.2f, 1.0f));
            else
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

            if (ImGui::Button(isDirectory ? "[Dir]" : "[File]", ImVec2(thumbnailSize, thumbnailSize)))
            {
                if (isDirectory)
                {
                    m_currentProjectDirectory /= path.filename();
                }

                else
                {
                    std::cout << "Selected File: " << filename << std::endl;

                    if (ext == ".scene")
                    {

                        std::string sceneName = filename.substr(0, filename.find_last_of('.'));

                        //SceneManager::Instance().LoadScene(sceneName);
                    }

                    m_selectedAssetPath = path.string();
                    m_selectedGameObject = nullptr;
                }
            }
            ImGui::PopStyleColor();

            if (ImGui::BeginDragDropSource())
            {
                std::string itemPath = path.string();
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
            }
            ImGui::EndMenu();
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
