#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"
#include "ReflectionMacros.h"


class ForwardMover : public MonoBehaviour
{
    DTGENERATED_BODY(ForwardMover);

public:
    ForwardMover() = default;
    virtual ~ForwardMover() = default;

    void Update(float deltaTime) override;

    void SetMoveSpeed(float speed) { m_moveSpeed = speed; }

private:
    float m_moveSpeed = 10.0f; // 이동 속도
};