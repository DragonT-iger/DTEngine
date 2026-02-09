#pragma once
#include <vector>
#include "MonoBehaviour.h"
#include "Unit.h"
#include "AllyUnit.h"
#include "EnemyUnit.h"
#include "AliceUnit.h"
#include "RedQueen.h"
#include "BattleGrid.h"


class Unit;
class AllyUnit;
class EnemyUnit;
class AliceUnit;
class RedQueenUnit;
class BattleGrid;
class GameObject;
//class Prefab;

struct UnitStats;

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
	bool MoveAndBattlePhase(float dTime);
	bool EndPhase();

	void Process(float dTime);

	// 준비준비 단계
	bool CanActuallyAttack(const Unit* me, const Unit* target) const; // 실질적인 공격이 가능한지.
	void GetAttackableTargets(Unit* me, std::vector<Unit*>& outTargets) const; // 공격 가능한 상대 목록.
	Unit* SelectAttackTarget(Unit* me, const std::vector<Unit*>& targets, int battleRule) const; // 우선순위에 따라 공격목표 설정.

	// 이동 준비 단계
	bool IsBattleUnit(const Unit* u) const; // 전투목록에 있는 유닛인지.
	Unit* FindNearestEnemy(const Vector2& from) const; // 가장 가까이 있는 적 찾기.
	Unit* FindNearestAlly(const AllyUnit* me, const Vector2& from) const; // 가장 가까이 있는 아군 찾기.
	Vector2 DecideMoveTarget_Ally(AllyUnit* me) const;
	Vector2 DecideMovePos_Ally(AllyUnit* me, const Vector2& moveTarget);
	Vector2 DecideMovePos_Enemy(EnemyUnit* me);

	// 전투 준비 단계
	Unit* FindBlockingUnitByPos(const Vector2& pos, Unit* me) const;
	Vector2 DecideAttackPos(Unit* me);

	// 이동 및 전투 단계
	void ResolveTurnAction(Unit* me);
	float CalculateDamage(Unit* me, Unit* target);

public:
	// 이번 스테이지 그리드 등록용. 
	void SetBattleGrid(BattleGrid* bg) { battleGrid = bg; }
	BattleGrid* GetBattleGrid() { return battleGrid; }

	// 이번 스테이지 앨리스 등록용.
	void SetAliceUnit(AliceUnit* aliceUnit) { m_aliceUnit = aliceUnit; }

	// 이번 스테이지 붉은여왕 등록용.
	void SetRedQueenUnit(RedQueenUnit* redQueenUnit) { m_redQueenUnit = redQueenUnit; }
	 
	// 이번 스테이지 아군, 적군 등록용. 어차피 각각 3개가 최대니까.. 
	void SetAllyUnit0(AllyUnit* allyUnit) { allyUnit0 = allyUnit; }
	void SetAllyUnit1(AllyUnit* allyUnit) { allyUnit1 = allyUnit; }
	void SetAllyUnit2(AllyUnit* allyUnit) { allyUnit2 = allyUnit; }

	// 튜토리얼에서 가져와서 방향 돌리기 위한용
	AllyUnit* GetAllyUnit0() { return allyUnit0; }

	void AddAllyUnit(AllyUnit* allyUnit);
	void AddEnemyUnit(EnemyUnit* enemyUnit);

	void SetEnemyUnit0(EnemyUnit* enemyUnit) { enemyUnit0 = enemyUnit; }
	void SetEnemyUnit1(EnemyUnit* enemyUnit) { enemyUnit1 = enemyUnit; }
	void SetEnemyUnit2(EnemyUnit* enemyUnit) { enemyUnit2 = enemyUnit; }

	EnemyUnit* GetEnemyUnit0() { return enemyUnit0; }

	void PrintFrame();
private:
	Phase m_currPhase = Phase::Ready;
	StageResult m_stageResult = StageResult::InProgress;
	bool m_isStageStart = false;	// 
	bool m_phaseEntered = false; // 페이즈 처음 들어온거 확인용.
	bool m_stageEnd = false; // 스테이지 끝났는지 확인용.

	float m_trapDamage = 30.0f; // 트랩 데미지.

	AliceUnit* m_aliceUnit = nullptr; // 앨리스 // 얘도 등록받아야해. 
	RedQueenUnit* m_redQueenUnit = nullptr; // 붉은여왕

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

	//Prefab* asdf = nullptr;
	int allyCount = 0;
	int enemyCount = 0;

	GameObject* m_victoryBG = nullptr;
	GameObject* m_loseBG = nullptr;
};