#pragma once
#include "ICommand.h"
#include "GameObject.h"
#include "Component.h"
#include <memory>

class RemoveComponentCommand : public ICommand
{
public:
    RemoveComponentCommand(GameObject* target, Component* componentToRemove)
        : m_targetGO(target), m_rawPtr(componentToRemove)
    {
    }

    void Execute() override
    {
        if (m_targetGO && m_rawPtr)
        {
            m_component = m_targetGO->_Internal_RemoveComponent(m_rawPtr);
        }
    }

    void Undo() override
    {
        if (m_targetGO && m_component)
        {
            m_targetGO->_Internal_AddComponent(std::move(m_component));
        }
    }

private:
    GameObject* m_targetGO;
    std::unique_ptr<Component> m_component; 
    Component* m_rawPtr; 
};