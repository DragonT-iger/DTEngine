#pragma once
#include "ICommand.h"
#include "Scene.h"
#include "GameObject.h"

class PasteGameObjectCommand : public ICommand
{
public:
    PasteGameObjectCommand(Scene* scene, GameObject* source)
        : m_scene(scene), m_rawPtr(nullptr)
    {
        if (source)
        {
            m_clonedObject = source->Clone();
            m_rawPtr = m_clonedObject.get();
        }
    }

    void Execute() override
    {
        if (m_clonedObject)
        {
            m_scene->_Internal_AddGameObject(std::move(m_clonedObject));
        }
    }

    void Undo() override
    {
        if (m_rawPtr)
        {
            m_clonedObject = m_scene->_Internal_RemoveGameObject(m_rawPtr);
        }
    }

    GameObject* GetPastedObject() const { return m_rawPtr; }

private:
    Scene* m_scene;
    std::unique_ptr<GameObject> m_clonedObject; 
    GameObject* m_rawPtr;
};