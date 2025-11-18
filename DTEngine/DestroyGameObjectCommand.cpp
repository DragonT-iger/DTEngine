#include "pch.h"
#include "DestroyGameObjectCommand.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"

DestroyGameObjectCommand::DestroyGameObjectCommand(Scene* scene, GameObject* target)
    : m_scene(scene), m_rootTarget(target)
{
    if (m_rootTarget)
    {
        m_rootTargetTf = m_rootTarget->GetTransform();
        if (m_rootTargetTf)
        {
            m_oldParent = m_rootTargetTf->GetParent();
        }

        CollectHierarchy(m_rootTarget, m_targetPtrs);
    }
}

void DestroyGameObjectCommand::CollectHierarchy(GameObject* target, std::vector<GameObject*>& outList)
{
    if (!target) return;

    outList.push_back(target);

    Transform* tf = target->GetTransform();
    if (tf)
    {
        const auto& children = tf->GetChildren();
        for (Transform* childTf : children)
        {
            GameObject* childGO = childTf->_GetOwner();
            if (childGO)
            {
                CollectHierarchy(childGO, outList);
            }
        }
    }
}

void DestroyGameObjectCommand::Execute()
{
    if (!m_scene || m_targetPtrs.empty()) return;

    if (m_rootTargetTf)
    {
        m_rootTargetTf->SetParent(nullptr, false);
    }

    for (GameObject* ptr : m_targetPtrs)
    {
        std::unique_ptr<GameObject> extracted = m_scene->_Internal_RemoveGameObject(ptr);
        if (extracted)
        {
            m_storedObjects.push_back(std::move(extracted));
        }
    }
}

void DestroyGameObjectCommand::Undo()
{
    if (!m_scene || m_storedObjects.empty()) return;

    for (auto& obj : m_storedObjects)
    {
        if (obj)
        {
            m_scene->_Internal_AddGameObject(std::move(obj));
        }
    }
    m_storedObjects.clear(); 

    if (m_rootTargetTf)
    {
        m_rootTargetTf->SetParent(m_oldParent, false);
    }
}