#include "EnemyUnit.h"
#include "GameObject.h"

BEGINPROPERTY(EnemyUnit)
DTPROPERTY_ACCESSOR(EnemyUnit, m_type, GetUnitType, SetUnitType)
DTPROPERTY_ACCESSOR(EnemyUnit, m_isBoss, IsBoss, SetBoss)
DTPROPERTY_ACCESSOR(EnemyUnit, m_pos, GetPos, SetPos)
ENDPROPERTY()

void EnemyUnit::SetBoss(bool isBoss)
{
	m_isBoss = isBoss;
	if (isBoss)
	{
		UnitStats bossStats = GetStats() * m_BossScale; // 스텟 뻥튀기
		SetStats(bossStats);
	}
	else
	{
		SetUnitType(m_type); // 다시 원래 스텟으로 돌리기
	}
}
