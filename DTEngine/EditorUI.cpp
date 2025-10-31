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
            void* data = prop.m_getter(comp);
            const auto& type = prop.m_type;
            const char* name = prop.m_name.c_str();

            // Vector3 처리
            if (type == typeid(Vector3))
            {
                Vector3 temp = *static_cast<Vector3*>(data);
                if (ImGui::DragFloat3(name, &temp.x, 0.1f))
                {
                    prop.m_setter(comp, &temp);
                }
            }

            else if (type == typeid(Quaternion))
            {
                // 이 컴포넌트가 Transform인지 확인
                Transform* tf = dynamic_cast<Transform*>(comp);

                if (tf && prop.m_name == "m_rotation")
                {

                    if (!ImGui::IsItemActive())
                    {
                        m_cachedEulerRotation = tf->GetRotationEuler();

                        
                    }
                    else {
						std::cout << "Transform m_rotation is active." << std::endl;
                    }

                    if (ImGui::DragFloat3("m_rotation", &m_cachedEulerRotation.x, 0.5f))
                    {
                        tf->SetRotationEuler(m_cachedEulerRotation);

                        std::cout << "Cached Euler Rotation: "
                            << m_cachedEulerRotation.x << ", "
                            << m_cachedEulerRotation.y << ", "
                            << m_cachedEulerRotation.z << std::endl;
                    }
                }
                else
                {
                    // Transform이 아니거나 m_rotation이 아닌 Quaternion은 그냥 DragFloat4로 표시
                    Quaternion temp = *static_cast<Quaternion*>(data);
                    if (ImGui::DragFloat4(name, &temp.x, 0.1f))
                    {
                        prop.m_setter(comp, &temp);
                    }
                }
            }

            else if (type == typeid(Transform*))
            {
                //Transform* parentTf = *static_cast<Transform**>(data);
                //std::string parentName = (parentTf) ? parentTf->_GetOwner()->GetName() : "None (Root)";
                //ImGui::Text("%s: %s", name, parentName.c_str());
            }
            // ...
        }
    }
}