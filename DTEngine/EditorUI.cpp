#include "pch.h"


#include <imgui.h>
#include <string>
#include <cctype>

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


static ImGuizmo::OPERATION m_currentOperation = ImGuizmo::TRANSLATE;
static ImGuizmo::MODE      m_currentMode      = ImGuizmo::LOCAL;

EditorUI::EditorUI() = default;
EditorUI::~EditorUI() = default;

void EditorUI::RenderToolbar(Game::EngineMode currentMode, std::function<void(Game::EngineMode)> onModeChanged)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);   
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 4.0f)); 

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    auto& colors = ImGui::GetStyle().Colors;
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(colors[ImGuiCol_ButtonHovered].x, colors[ImGuiCol_ButtonHovered].y, colors[ImGuiCol_ButtonHovered].z, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(colors[ImGuiCol_ButtonActive].x, colors[ImGuiCol_ButtonActive].y, colors[ImGuiCol_ButtonActive].z, 0.5f));


    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));

    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 30.0f));


    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDecoration               
        | ImGuiWindowFlags_NoMove                   
        | ImGuiWindowFlags_NoScrollbar              
        | ImGuiWindowFlags_NoSavedSettings          
        | ImGuiWindowFlags_NoBringToFrontOnFocus;   


    if (ImGui::Begin("MainToolbar", nullptr, window_flags))
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
    }

    ImGui::End();

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(3);
}

void EditorUI::Render(Scene* activeScene)
{
    if (!activeScene) return;
    DrawHierarchyWindow(activeScene);
    DrawInspectorWindow();
    //DrawGizmo(activeScene);

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

    
    //DrawOverlay();
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

            Vector3 targetPos = tf->GetPosition();

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

    if (ImGui::IsItemClicked())
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
            HistoryManager::Instance().Do(std::move(cmd));
        }
        ImGui::EndPopup();
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
        ImGui::EndDragDropTarget();
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
    ImGui::End();
}


void EditorUI::DrawInspectorWindow()
{
    ImGui::Begin("Inspector");

    if (!m_selectedGameObject)
    {
        ImGui::Text("No object selected.");
        ImGui::End();
        return;
    }

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
            if (typeName == "Transform")
            {
                continue;
            }

            if (ImGui::Selectable(typeName.c_str()))
            {
                auto cmd = std::make_unique<AddComponentCommand>(m_selectedGameObject, typeName);
                HistoryManager::Instance().Do(std::move(cmd));
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
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
    bool isCamera = (dynamic_cast<Camera*>(comp) != nullptr);

    if (ImGui::BeginPopupContextItem("ComponentContextMenu"))
    {
        if (isTransform || isCamera)
        {
            if (ImGui::MenuItem("Align With View"))
            {
                if (m_sceneCamera && comp->_GetOwner())
                {
                    Transform* targetTf = comp->_GetOwner()->GetTransform();
                    Transform* camTf = m_sceneCamera->_GetOwner()->GetTransform();

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
                // DragInt는 uint64_t를 지원하지 않으므로 InputScalar 사용
                uint64_t temp = *static_cast<uint64_t*>(data);
                if (ImGui::InputScalar(name, ImGuiDataType_U64, &temp, nullptr, nullptr, "%llu", ImGuiInputTextFlags_ReadOnly))
                {
                    // (읽기 전용이므로 Setter 호출 안 함)
                }
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
            else {
                std::cout << "타입 미구현: " << type.name() << std::endl;
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

    ImGui::End();
    ImGui::PopStyleVar();
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
