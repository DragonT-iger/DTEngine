#include "pch.h"
#include "move.h"
#include "Transform.h"
#include "GameObject.h"

BEGINPROPERTY(move)
DTPROPERTY(move, m_speed)
ENDPROPERTY()

void move::Update(float deltaTime)
{
    Transform* transform = GetTransform();
    if (!transform) return;

    Vector3 currentPos = transform->GetPosition();

    Vector3 nextEuler = currentPos + (m_speed * deltaTime);

    // 적용
    transform->SetPosition(nextEuler);
}
