#pragma once
#include "ICommand.h"
#include "MonoBehaviour.h" 

class ChangeComponentActiveCommand : public ICommand
{
public:
    ChangeComponentActiveCommand(MonoBehaviour* target, bool oldState, bool newState)
        : m_target(target),
        m_oldState(oldState),
        m_newState(newState)
    {
    }

    void Execute() override
    {
        if (m_target)
        {
            m_target->SetActive(m_newState);
        }
    }

    void Undo() override
    {
        if (m_target)
        {
            m_target->SetActive(m_oldState);
        }
    }

private:
    MonoBehaviour* m_target;
    bool m_oldState;
    bool m_newState;
};