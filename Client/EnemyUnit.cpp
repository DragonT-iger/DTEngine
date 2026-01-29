#include "EnemyUnit.h"
#include "GameObject.h"

BEGINPROPERTY(EnemyUnit)
DTPROPERTY(EnemyUnit, path0)
DTPROPERTY(EnemyUnit, path1)
DTPROPERTY(EnemyUnit, path2)
DTPROPERTY(EnemyUnit, path3)
DTPROPERTY(EnemyUnit, path4)
DTPROPERTY(EnemyUnit, path5)
DTPROPERTY(EnemyUnit, path6)
DTPROPERTY(EnemyUnit, path7)
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
		bossStats.attackRange = GetStats().attackRange; // 단 공격범위는 예외.
		SetStats(bossStats);
	}
	else
	{
		SetUnitType(m_type); // 다시 원래 스텟으로 돌리기
	}
}

Vector2 EnemyUnit::GetPathPoint(int i) const
{
	switch (i)
	{
	case 0: return path0;
	case 1: return path1;
	case 2: return path2;
	case 3: return path3;
	case 4: return path4;
	case 5: return path5;
	case 6: return path6;
	case 7: return path7;
	default: return GRIDPOS_INVALID;
	}
}
