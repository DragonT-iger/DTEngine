#pragma once
#include "ICommand.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentFactory.h"
#include "IDManager.h"
#include <string>
#include <memory>

class AddComponentCommand : public ICommand
{
public:
    AddComponentCommand(GameObject* target, const std::string& componentTypeName)
        : m_targetGO(target)
    {
        m_component = ComponentFactory::Instance().Create(componentTypeName);
        m_rawPtr = m_component.get();
    }

    void Execute() override
    {
        if (m_targetGO && m_component)
        {
            m_targetGO->_Internal_AddComponent(std::move(m_component));
        }
    }

    void Undo() override
    {
        if (m_targetGO && m_rawPtr)
        {
            m_component = m_targetGO->_Internal_RemoveComponent(m_rawPtr);
        }
    }

    Component* GetCreatedComponent() const { return m_rawPtr; }

private:
    GameObject* m_targetGO;
    std::unique_ptr<Component> m_component;
    Component* m_rawPtr;
};