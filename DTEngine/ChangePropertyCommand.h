#pragma once
#include "ICommand.h"
#include "Component.h"
#include <functional>
#include <memory>

template<typename T>
class ChangePropertyCommand : public ICommand
{
public:
    ChangePropertyCommand(
        Component* target,
        std::function<void(void*, void*)> setter,
        const T& oldValue,
        const T& newValue)
        : m_target(target),
        m_setter(setter),
        m_oldValue(oldValue),
        m_newValue(newValue)
    {
    }

    void Execute() override
    {
        m_setter(m_target, (void*)&m_newValue);
    }

    void Undo() override
    {
        m_setter(m_target, (void*)&m_oldValue);
    }

private:
    Component* m_target;
    std::function<void(void*, void*)> m_setter;
    T m_oldValue;
    T m_newValue;
};