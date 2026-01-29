#include "AliceUnit.h"
#include "GameObject.h"

BEGINPROPERTY(AliceUnit)
DTPROPERTY_ACCESSOR(AliceUnit, m_pos, GetPos, SetPos)
ENDPROPERTY()

AliceUnit::AliceUnit()
{
	SetTeam(Team::Ally);
	SetUnitType(UnitType::Alice);
}
