#pragma once


#include "SimpleMathHelper.h"
#include "ICommand.h"
#include "Transform.h"

class ChangeEditorTransformCommand : public ICommand
{
public:
    ChangeEditorTransformCommand(Transform* target,
        const Vector3& oldPos, const Vector3& newPos,
        const Vector3& oldRot, const Vector3& newRot,
        const Vector3& oldScale, const Vector3& newScale)
        : m_target(target),
        m_oldPos(oldPos), m_newPos(newPos),
        m_oldRot(oldRot), m_newRot(newRot),
        m_oldScale(oldScale), m_newScale(newScale) {
    }

    void Execute() override
    {
        m_target->SetPosition(m_newPos);
        m_target->SetRotationEuler(m_newRot);
        m_target->SetScale(m_newScale);
    }

    void Undo() override
    {
        m_target->SetPosition(m_oldPos);
        m_target->SetRotationEuler(m_oldRot);
        m_target->SetScale(m_oldScale);
    }

private:
    Transform* m_target;
    Vector3 m_oldPos, m_newPos;
    Vector3 m_oldRot, m_newRot;
    Vector3 m_oldScale, m_newScale;
};

