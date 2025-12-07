#pragma once
#include "ICommand.h"
#include "Scene.h"
#include "GameObject.h"
#include "IDManager.h" 

class CreateGameObjectCommand : public ICommand
{
public:
    CreateGameObjectCommand(Scene* scene, const std::string& name = "GameObject")
        : m_scene(scene)
    {
        m_gameObject = std::make_unique<GameObject>(name);

        m_gameObject->_SetID(IDManager::Instance().GetNewUniqueID());
        m_rawPtr = m_gameObject.get();
    }

    void Execute() override
    {
        m_scene->_Internal_AddGameObject(std::move(m_gameObject));
    }

    void Undo() override
    {
        m_gameObject = m_scene->_Internal_RemoveGameObject(m_rawPtr);
    }

    GameObject* GetCreatedGameObject() const
    {
        return m_rawPtr;
	}

private:
    Scene* m_scene;
    std::unique_ptr<GameObject> m_gameObject;
    GameObject* m_rawPtr;
};