#pragma once
#include "ICommand.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h" 
#include <memory>

class DestroyGameObjectCommand : public ICommand
{
public:
    DestroyGameObjectCommand(Scene* scene, GameObject* target)
        : m_scene(scene), m_rawPtr(target)
    {
        if (m_rawPtr)
        {
            m_targetTf = m_rawPtr->GetTransform();
            if (m_targetTf)
            {
                m_oldParent = m_targetTf->GetParent();
            }
        }
    }

    void Execute() override
    {
        if (m_scene && m_rawPtr)
        {
            if (m_targetTf)
            {
                m_targetTf->SetParent(nullptr, false);
            }

            m_gameObject = m_scene->_Internal_RemoveGameObject(m_rawPtr);
        }
    }

    void Undo() override
    {
        if (m_scene && m_gameObject)
        {
            m_scene->_Internal_AddGameObject(std::move(m_gameObject));

            if (m_targetTf)
            {
                m_targetTf->SetParent(m_oldParent, false);
            }
        }
    }

private:
    Scene* m_scene;
    std::unique_ptr<GameObject> m_gameObject;
    GameObject* m_rawPtr;

    Transform* m_targetTf = nullptr;
    Transform* m_oldParent = nullptr;
};