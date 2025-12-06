#pragma once
#include "MonoBehaviour.h"
#include <DirectXTK/SimpleMath.h>
#include "ReflectionMacros.h"

using Vector3 = DirectX::SimpleMath::Vector3;

class move : public MonoBehaviour
{
    DTGENERATED_BODY(move);

public:

    void Update(float deltaTime) override;

private:
    Vector3 m_speed = Vector3(1.0f, 2.0f, 1.0f);
};

