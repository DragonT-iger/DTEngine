#include "CombatTestRunner.h"
#include "GameObject.h"

BEGINPROPERTY(CombatTestRunner)
DTPROPERTY_SETTER(CombatTestRunner, m_grid, SetBattleGrid)
DTPROPERTY_SETTER(CombatTestRunner, m_controller, SetCombatController)

DTPROPERTY_SETTER(CombatTestRunner, m_ally0, SetAllyUnit0)
DTPROPERTY_SETTER(CombatTestRunner, m_ally1, SetAllyUnit1)
DTPROPERTY_SETTER(CombatTestRunner, m_ally2, SetAllyUnit2)

DTPROPERTY_SETTER(CombatTestRunner, m_enemy0, SetEnemyUnit0)
DTPROPERTY_SETTER(CombatTestRunner, m_enemy1, SetEnemyUnit1)
DTPROPERTY_SETTER(CombatTestRunner, m_enemy2, SetEnemyUnit2)

DTPROPERTY_SETTER(CombatTestRunner, m_alice, SetAliceUnit)
ENDPROPERTY()