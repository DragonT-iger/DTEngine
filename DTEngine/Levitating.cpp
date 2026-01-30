#include "pch.h"
#include "Levitating.h"


BEGINPROPERTY(Levitating)
DTPROPERTY(Levitating, m_Boundary)
DTPROPERTY(Levitating, m_Speed)

ENDPROPERTY()

void Levitating::Start()
{
	m_InitialY = GetTransform()->GetWorldPosition().y;
}

void Levitating::Update(float deltaTime)
{
    m_TimeAcc += deltaTime * m_Speed;

    float sinValue = sinf(m_TimeAcc);

    Vector3 currentPos = GetTransform()->GetWorldPosition();
    currentPos.y = m_InitialY + (sinValue * m_Boundary);

    GetTransform()->SetPosition(currentPos);
}

