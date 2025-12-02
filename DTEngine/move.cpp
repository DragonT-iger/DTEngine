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

    transform->SetPosition(nextEuler);
    

    //std::cout << this->_GetOwner()->GetName() << std::endl;
}
