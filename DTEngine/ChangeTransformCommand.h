#pragma once
#include "ICommand.h"
#include "Transform.h"

class ChangeTransformCommand : public ICommand
{
public:
    ChangeTransformCommand(Transform* target,
        const Vector3& oldPos, const Vector3& newPos,
        const Quaternion& oldRot, const Quaternion& newRot,
        const Vector3& oldScale, const Vector3& newScale)
        : m_target(target),
        m_oldPos(oldPos), m_newPos(newPos),
        m_oldRot(oldRot), m_newRot(newRot),
        m_oldScale(oldScale), m_newScale(newScale) {
    }

    void Execute() override
    {
        m_target->SetPosition(m_newPos);
        m_target->SetRotationQuat(m_newRot);
        m_target->SetScale(m_newScale);
    }

    void Undo() override
    {
        m_target->SetPosition(m_oldPos);
        m_target->SetRotationQuat(m_oldRot);
        m_target->SetScale(m_oldScale);
    }

private:
    Transform* m_target;
    Vector3 m_oldPos, m_newPos;
    Quaternion m_oldRot, m_newRot;
    Vector3 m_oldScale, m_newScale;
};

