#pragma once
#include <vector>

#include "Unit.h"
#include "AllyUnit.h"
#include "EnemyUnit.h"
#include "BattleGrid.h"

class Unit;
class AllyUnit;
class EnemyUnit;
class BattleGrid;

enum class Phase // 턴 관리용
{
	Ready,
	ReadyToMove,
	ReadyForBattle,
	MoveAndBattle,
	End
};

enum class StageResult // 스테이지 상태
{
	InProgress, // 진행중
	Win,        // 승리
	Lose        // 패배
};

class CombatController : public MonoBehaviour
{
	DTGENERATED_BODY(CombatController);

public:
	CombatController() = default;
	~CombatController();

	void Start() override;
	void Update(float dTime) override;

public:
	void Setup();

	bool ReadyPhase();
	bool ReadyToMovePhase();
	bool ReadyForBattlePhase();
	bool MoveAndBattlePhase();
	bool EndPhase();

	void Process();

	bool CanActuallyAttack(const Unit* me, const Unit* target) const; // 실질적인 공격이 가능한지.
	void GetAttackableTargets(Unit* me, std::vector<Unit*>& outTargets) const; // 공격 가능한 상대 목록.
	Unit* SelectAttackTarget(Unit* me, const std::vector<Unit*>& targets) const; // 우선순위에 따라 공격목표 설정.

	bool IsBattleUnit(const Unit* u) const; // 전투목록에 있는 유닛인지.
	Vector2 FindNearestEnemyPos(const Vector2& from) const; // 가장 가까이 있는 적 찾기.
	Vector2 FindNearestAllyPos(const AllyUnit* me, const Vector2& from) const; // 가장 가까이 있는 아군 찾기.
	Vector2 DecideMoveTarget_Ally(AllyUnit* me) const;

public:
	// 이번 스테이지 그리드 등록용. 
	void SetBattleGrid(BattleGrid* bg) { battleGrid == bg; }
	 
	// 이번 스테이지 아군, 적군 등록용. 어차피 각각 3개가 최대니까.. 
	void SetAllyUnit0(AllyUnit* allyUnit) { allyUnit0 = allyUnit; }
	void SetAllyUnit1(AllyUnit* allyUnit) { allyUnit1 = allyUnit; }
	void SetAllyUnit2(AllyUnit* allyUnit) { allyUnit2 = allyUnit; }

	void SetEnemyUnit0(EnemyUnit* enemyUnit) { enemyUnit0 = enemyUnit; }
	void SetEnemyUnit1(EnemyUnit* enemyUnit) { enemyUnit1 = enemyUnit; }
	void SetEnemyUnit2(EnemyUnit* enemyUnit) { enemyUnit2 = enemyUnit; }

private:
	Phase m_currPhase = Phase::Ready;
	StageResult m_stageResult = StageResult::InProgress;

	std::vector<AllyUnit*> m_allyUnits; // 아군유닛 목록
	std::vector<EnemyUnit*> m_enemyUnits; // 적군유닛 목록

	std::vector<Unit*> m_battleUnits; // 전투유닛 목록

	// 이번 스테이지 그리드 등록용.
	BattleGrid* battleGrid = nullptr;

	// 이번 스테이지 아군 등록용
	AllyUnit* allyUnit0 = nullptr;
	AllyUnit* allyUnit1 = nullptr;
	AllyUnit* allyUnit2 = nullptr;

	// 이번 스테이지 적군 등록용
	EnemyUnit* enemyUnit0 = nullptr;
	EnemyUnit* enemyUnit1 = nullptr;
	EnemyUnit* enemyUnit2 = nullptr;
};