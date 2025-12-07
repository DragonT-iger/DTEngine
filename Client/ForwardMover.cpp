#include "ForwardMover.h"
#include "GameObject.h"
#include "Transform.h"

BEGINPROPERTY(ForwardMover)
DTPROPERTY(ForwardMover, m_moveSpeed)
ENDPROPERTY()

void ForwardMover::Update(float deltaTime)
{
    Transform* tr = GetTransform();
    if (!tr) return;

    Vector3 currentPos = tr->GetPosition();

    Vector3 forwardDir = tr->Forward();

    Vector3 nextPos = currentPos + (forwardDir * m_moveSpeed * deltaTime);

    tr->SetPosition(nextPos);
}