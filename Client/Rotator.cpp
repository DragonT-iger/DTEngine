#include "Rotator.h"
#include "GameObject.h" 
#include "Transform.h"

BEGINPROPERTY(Rotator)
DTPROPERTY(Rotator, m_rotationSpeed)
ENDPROPERTY()

void Rotator::Update(float deltaTime)
{
    Transform* tr = GetTransform();
    if (!tr) return;

    Vector3 currentEuler = tr->GetEditorEuler();

    currentEuler += m_rotationSpeed * deltaTime;

    tr->SetRotationEuler(currentEuler);
}