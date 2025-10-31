#include "pch.h"
#include "EditorUI.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h" 
#include "ReflectionDatabase.h"

#include <imgui.h>
#include <string>

EditorUI::EditorUI() = default;
EditorUI::~EditorUI() = default;

void EditorUI::Render(Scene* activeScene)
{
    if (!activeScene) return;
    DrawHierarchyWindow(activeScene);
    DrawInspectorWindow();
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
    const ClassInfo* info = ReflectionDatabase::Instance().GetClassInfomation(comp->_GetTypeName());
    if (!info) return;

    if (ImGui::CollapsingHeader(comp->_GetTypeName()))
    {
        for (const PropertyInfo& prop : info->m_properties)
        {
            // [m_editorEulerAngles] �Ӽ��� �ǳʶٱ�
            if (prop.m_name == "m_editorEulerAngles")
            {
                continue;
            }

            void* data = prop.m_getter(comp);
            const auto& type = prop.m_type;
            const char* name = prop.m_name.c_str();


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
            // uint64_t (ID ��)
            else if (type == typeid(uint64_t))
            {
                // DragInt�� uint64_t�� �������� �����Ƿ� InputScalar ���
                uint64_t temp = *static_cast<uint64_t*>(data);
                if (ImGui::InputScalar(name, ImGuiDataType_U64, &temp, nullptr, nullptr, "%llu", ImGuiInputTextFlags_ReadOnly))
                {
                    // (�б� �����̹Ƿ� Setter ȣ�� �� ��)
                }
            }
            // std::string
            else if (type == typeid(std::string))
            {
                std::string temp = *static_cast<std::string*>(data);
                char buffer[256]; // �ӽ� ����
                strncpy_s(buffer, temp.c_str(), sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0'; // �� ���� ����

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
            // Vector4 (�߰�)
            else if (type == typeid(Vector4))
            {
                Vector4 temp = *static_cast<Vector4*>(data);
                if (ImGui::DragFloat4(name, &temp.x, 0.1f))
                {
                    prop.m_setter(comp, &temp);
                }
            }
            // Quaternion (m_rotation Ư�� ó��)
            else if (type == typeid(Quaternion))
            {
                Transform* tf = dynamic_cast<Transform*>(comp);

                if (tf && prop.m_name == "m_rotation")
                {
                    // (m_cachedEulerRotation ������ ����)
                    Vector3 displayEuler = tf->GetEditorEuler();
                    if (ImGui::DragFloat3("Rotation", &displayEuler.x, 0.5f))
                    {
                        tf->SetRotationEuler(displayEuler);
                    }
                }
                else // m_rotation�� �ƴ� �ٸ� ���ʹϾ�
                {
                    Quaternion temp = *static_cast<Quaternion*>(data);
                    if (ImGui::DragFloat4(name, &temp.x, 0.1f))
                    {
                        prop.m_setter(comp, &temp);
                    }
                }
            }
            // Transform* (������)
            else if (type == typeid(Transform*))
            {
                //Transform* parentTf = *static_cast<Transform**>(data);
                //std::string parentName = (parentTf) ? parentTf->_GetOwner()->GetName() : "None (Root)";
                //ImGui::Text("%s: %s", name, parentName.c_str());
            }
            else {
				std::cout << "Ÿ�� �̱���: " << type.name() << std::endl;
            }
        }
    }
}