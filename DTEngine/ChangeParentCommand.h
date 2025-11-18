#pragma once
#include "ICommand.h"
#include "Transform.h"

class ChangeParentCommand : public ICommand
{
public:
    ChangeParentCommand(Transform* target, Transform* oldParent, Transform* newParent)
        : m_target(target),
        m_oldParent(oldParent),
        m_newParent(newParent)
    {
    }

    void Execute() override
    {
        if (m_target)
        {
            // SetParent의 worldPositionStays를 true로 설정
            if (!m_target->SetParent(m_newParent, true)) {
				isSuccess = false;
            }
        }
    }

    void Undo() override
    {
        if (m_target)
        {
            // Undo할 때도 월드 좌표 기준으로 부모를 복원
            m_target->SetParent(m_oldParent, isSuccess);
        }
    }

private:
	bool isSuccess = true;
    Transform* m_target;
    Transform* m_oldParent;
    Transform* m_newParent;
};