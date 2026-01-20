#include "Rotator.h"
#include "GameObject.h" 
#include "Transform.h"

BEGINPROPERTY(Rotator)
DTPROPERTY(Rotator, m_rotationSpeed)
DTPROPERTY(Rotator, m_go)
DTPROPERTY(Rotator, m_rotator)
DTPROPERTY(Rotator, col)
ENDPROPERTY()

void Rotator::Update(float deltaTime)
{
    Transform* tr = GetTransform();
    if (!tr) return;;

    Vector3 currentEuler = tr->GetEditorEuler();

    currentEuler += m_rotationSpeed * deltaTime;

    tr->SetRotationEuler(currentEuler);
}

//void Rotator::Start()
//{
//	std::cout << "Started Rotator on " << GetName() << std::endl;
//}
