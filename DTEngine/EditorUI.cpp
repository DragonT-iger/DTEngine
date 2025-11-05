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



static ImGuizmo::OPERATION m_currentOperation = ImGuizmo::TRANSLATE;
static ImGuizmo::MODE m_currentMode = ImGuizmo::LOCAL;

EditorUI::EditorUI() = default;
EditorUI::~EditorUI() = default;

void EditorUI::Render(Scene* activeScene)
{
    if (!activeScene) return;
    DrawHierarchyWindow(activeScene);
    DrawInspectorWindow();


    Camera* camera = activeScene->GetMainCamera();
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

            if (ImGuizmo::IsUsing())
            {
                worldMatrix = worldMatrix;

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

            draggedTf->SetParent(tf, true); 
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

    std::string sceneName = activeScene->GetName();

    ImGuiTreeNodeFlags sceneFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    bool scene_node_open = ImGui::TreeNodeEx(sceneName.c_str(), sceneFlags);

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DRAG_ITEM"))
        {
            IM_ASSERT(payload->DataSize == sizeof(Transform*));
            Transform* draggedTf = *(Transform**)payload->Data;

            if (draggedTf)
            {
                draggedTf->SetParent(nullptr, true);
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

    bool isActive = m_selectedGameObject->IsActive();
    if (ImGui::Checkbox("##ActiveCheckbox", &isActive)) 
    {
        m_selectedGameObject->SetActive(isActive);
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

    ImGui::End();
}

void EditorUI::DrawComponentProperties(Component* comp)
{
    if (!comp) return;

    ImGui::PushID(comp);

    const ClassInfo* info = ReflectionDatabase::Instance().GetClassInfomation(comp->_GetTypeName());
    if (!info) return;

    bool header_open = false;
    bool compIsActive = true;

    MonoBehaviour* mb = dynamic_cast<MonoBehaviour*>(comp);
    if (mb)
    {
        compIsActive = mb->IsActive();

        std::string chkID = "##active_";
        chkID += comp->_GetTypeName();

        if (ImGui::Checkbox(chkID.c_str(), &compIsActive))
        {
            mb->SetActive(compIsActive);
        }
        ImGui::SameLine(); 
    }


    if (!ImGui::CollapsingHeader(comp->_GetTypeName()))
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

            // float
            if (type == typeid(float))
            {
                float temp = *static_cast<float*>(data);
                if (ImGui::DragFloat(name, &temp, 0.1f))
                {
                    prop.m_setter(comp, &temp);
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
            }
            // bool
            else if (type == typeid(bool))
            {
                bool temp = *static_cast<bool*>(data);
                if (ImGui::Checkbox(name, &temp))
                {
                    prop.m_setter(comp, &temp);
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
            }


            // Vector3
            else if (type == typeid(Vector3))
            {
                Vector3 temp = *static_cast<Vector3*>(data);
                if (ImGui::DragFloat3(name, &temp.x, 0.1f))
                {
                    prop.m_setter(comp, &temp);
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
            }
            // Quaternion (m_rotation 특별 처리)
            else if (type == typeid(Quaternion))
            {
                Transform* tf = dynamic_cast<Transform*>(comp);

                if (tf && prop.m_name == "m_rotation")
                {
                    // (m_cachedEulerRotation 로직은 유지)
                    Vector3 displayEuler = tf->GetEditorEuler();
                    if (ImGui::DragFloat3("Rotation", &displayEuler.x, 0.5f))
                    {
                        tf->SetRotationEuler(displayEuler);
                    }
                }
                else // m_rotation이 아닌 다른 쿼터니언
                {
                    Quaternion temp = *static_cast<Quaternion*>(data);
                    if (ImGui::DragFloat4(name, &temp.x, 0.1f))
                    {
                        prop.m_setter(comp, &temp);
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