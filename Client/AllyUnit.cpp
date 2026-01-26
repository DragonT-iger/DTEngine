#include "AllyUnit.h"
#include "GameObject.h"

BEGINPROPERTY(AllyUnit)
DTPROPERTY_ACCESSOR(AllyUnit, m_type, GetUnitType, SetUnitType)
DTPROPERTY_ACCESSOR(AllyUnit, m_moveRule, GetMoveRule, SetMoveRule)
DTPROPERTY_ACCESSOR(AllyUnit, m_battleRule, GetBattleRule, SetBattleRule)
DTPROPERTY_ACCESSOR(AllyUnit, m_pos, GetPos, SetPos)
ENDPROPERTY()