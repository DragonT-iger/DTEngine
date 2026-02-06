#include "RedQueen.h"
#include "GameObject.h"

BEGINPROPERTY(RedQueenUnit)
DTPROPERTY_ACCESSOR(RedQueenUnit, m_pos, GetPos, SetPos)
ENDPROPERTY()

RedQueenUnit::RedQueenUnit()
{
	SetTeam(Team::Enemy);
	SetUnitType(UnitType::Queen);
}
