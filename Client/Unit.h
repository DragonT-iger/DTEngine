#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"
#include "Animatior.h"
#include "GameObject.h"

class Animator;

static constexpr Vector2 GRIDPOS_INVALID{ -999, -999 };

enum UnitType
{
    Rook = 0,
    Knight,
    Bishop,
    Alice,
    Queen
};

enum class TurnAction // 이번 턴에 할 행동
{
    Wait,
    Move,
    Attack,
    Miss,
    BreakWall,

    Die, // 사망처리용
};

enum class Team
{
    Ally,
    Enemy
};

enum Dir8
{
    Right,
    UpRight,
    Up,
    UpLeft,
    Left,
    DownLeft,
    Down,
    DownRight,
};

enum class ActionPhase
{
    None,
    Rotating,
    Moving,
    Attacking,
    Dying
};

struct MoveAnim
{
    bool active = false;
    float t = 0.0f;
    float duration = 1.3f;
    Vector3 from{};
    Vector3 to{};
};

struct RotateAnim
{
    bool active = false;
    float t = 0.0f;
    float duration = 0.1f;
    float fromYaw = 0.0f;  // degrees
    float toYaw = 0.0f;  // degrees
};



struct UnitStats // 유닛 기본 스텟
{
    int atk = 20;
    int def = 10;
    float maxHp = 100;
    int attackRange = 1;
};

// 유닛들이 공통적으로 들고있는 요소
class Unit : public MonoBehaviour 
{
public:
    Unit() { SetUnitType(UnitType::Rook); }
    virtual ~Unit() = default;

    void Start() override;

    // 팀
    Team GetTeam() const { return m_team; } 
    void SetTeam(Team team) { m_team = team; }

    // 유닛 타입 - 에디터에서 조절 가능하게
    const int& GetUnitType() const { return m_type; }
    void SetUnitType(int type);

    // 현재 위치 - 에디터에서 조절 가능하게
    const Vector2& GetPos() const { return m_pos; }
    void SetPos(const Vector2& p) { m_pos = p; }

    // 유닛 기본 스텟
    const UnitStats& GetStats() const { return m_stats; }
    void SetStats(const UnitStats& s);

    // 유닛 현재 체력
    float GetHp() const { return m_hp; }
    bool IsAlive() const { return m_isAlive; }
    void SetIsAlive(bool alive) { m_isAlive = alive; }

    // 데미지 받음
    void TakeDamage(float damage) { m_hp = (std::max)(m_hp - damage, 0.0f); }

    // 힐 받음
    void Heal(float heal) { m_hp = (std::min)(m_hp + heal, m_stats.maxHp); }

    // 이동목표
    Unit* GetMoveTarget() const { return m_moveTarget; }
    void SetMoveTarget(Unit* u) { m_moveTarget = u; }
    Vector2 GetMoveTargetPos() const { return m_moveTargetPos; }
    void SetMoveTargetPos(const Vector2& p) { m_moveTargetPos = p; }
    bool HasMoveTarget() const { return m_moveTargetPos != GRIDPOS_INVALID; }

    // 이동지점
    Vector2 GetMovePos() const { return m_movePos; }
    void SetMovePos(const Vector2& p) { m_movePos = p; }
    bool HasMovePos() const { return m_movePos != GRIDPOS_INVALID; }

    // 공격목표
    Unit* GetAttackTarget() const { return m_attackTarget; }
    void SetAttackTarget(Unit* u) { m_attackTarget = u; }
    bool HasAttackTarget() const { return m_attackTarget != nullptr; }

    // 공격지점
    Vector2 GetAttackPos() const { return m_attackPos; }
    void SetAttackPos(const Vector2& p) { m_attackPos = p; }
    bool HasAttackPos() const { return m_attackPos != GRIDPOS_INVALID; }

    // 이번 턴 행동
    TurnAction GetAction() const { return m_action; }
    void SetAction(TurnAction action) { m_action = action; }
    bool IsActionDone() const { return m_actionDone; }
    void SetActionDone(bool done) { m_actionDone = done; }

    void StartAction();
    void UpdateAction(float dTime);

    // 바라보는 방향
    Dir8 GetDir() const { return m_dir; }
    int GetDir2(const Vector2& p) const;
    void SetDir(const Vector2& p);
    
    // 인식범위
    int GetPerceptionRange() const { return m_perceptionRange; }
    
    // 목표, 지점, 행동 초기화
    void ResetTurnPlan();


    // 애니메이션용
    void BeginRotateToDir();
    bool TickRotate(float dt);

    void BeginMoveToPos();
    bool TickMove(float dt);

    void FinishAction();

    void PlayAnim(uint64_t id, uint64_t id1, uint64_t id2, float speed, bool loop);

    void StartIdleAnim();
    void StartMoveAnim();
    void StartAttackAnim();
    void StartDieAnim();

    bool IsAnimStart() const { return m_animStart; }
    bool IsAnimDone() { return m_anim->GetAnimationDone(); }

    bool IsOnTrapTile() const { return m_isOnTrapTile; }
    void SetOnTrapTile(bool on) { m_isOnTrapTile = on; }

private:
    static Vector3 GridToWorld(const Vector2& p);
    static float   DirToYaw(Dir8 d);
    static float   SmoothStep(float t);
    static float   SmoothStep2(float t, float start, float end);
    static float   LerpAngleDeg(float a, float b, float t);
    static float   NormalizeDeg(float deg);
    static float   DeltaAngleDeg(float from, float to);

protected:
    Team m_team = Team::Ally;
    int m_type = UnitType::Rook; // 이건 에디터에서 만지게 할거임

    Vector2 m_pos{}; // 이건 에디터에서 만지게 할거임

    UnitStats m_stats{};
    float m_hp = 100;
    bool m_isAlive = true;

    int m_perceptionRange = 3; // 인식범위

    Unit* m_moveTarget = nullptr;
    Vector2 m_moveTargetPos = GRIDPOS_INVALID;
    Vector2 m_movePos = GRIDPOS_INVALID;

    Unit* m_attackTarget = nullptr; 
    Vector2 m_attackPos = GRIDPOS_INVALID;

    TurnAction m_action = TurnAction::Wait;
    bool m_actionDone = false;

    Dir8 m_dir = Dir8::Down;

    bool m_isOnTrapTile = false;



private:
    ActionPhase m_phase = ActionPhase::None;

    RotateAnim m_rot;
    MoveAnim   m_move;

    bool m_animStart = false;
    Animator* m_anim = nullptr;
    Animator* m_anim1 = nullptr;
    Animator* m_anim2 = nullptr;

    std::string m_weaponName;
};