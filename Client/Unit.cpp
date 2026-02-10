#include "Unit.h"
#include "AssetDatabase.h"
#include "Dissolved.h"
#include "Effect.h"
#include "SoundManager.h"

static const UnitStats UnitStatsTable[] =
{
    // 공, 방, 체, 공격범위
    { 20, 10, 150, 1 }, // 룩 
    { 25,  5, 110, 1 }, // 나이트
    { 30,  2,  90, 2 }, // 비숍
    {  0,  0, 150, 0 }, // 앨리스
    {  0,  0, 150, 0 }, // 퀸
};

void Unit::Start()
{
    if (_GetOwner()->GetName() == "unit_Alice_ttttst3" || _GetOwner()->GetName() == "unit_Queen_tttst")
    {
        m_anim = _GetOwner()->GetComponent<Animator>();
    }
    
    Transform* tf = GetTransform();
    if (!tf) return;

    for (Transform* child : tf->GetChildren())
    {
        if (!child) continue;
        if (child->_GetOwner()->GetName() == "Chess")
        {
            m_anim = child->_GetOwner()->GetComponent<Animator>();
        }
        else if (child->_GetOwner()->GetName() == "Shield")
        {
            m_anim2 = child->_GetOwner()->GetComponent<Animator>();
        }
        else
        {
            Animator* anim = child->_GetOwner()->GetComponent<Animator>();
            if (anim)
            {
                m_weaponName = child->_GetOwner()->GetName();
                m_anim1 = child->_GetOwner()->GetComponent<Animator>();
            }
        }
    }
}

void Unit::SetUnitType(int type)
{
    if (type < UnitType::Rook || type > UnitType::Queen) type = UnitType::Rook;
    m_type = type;
    SetStats(UnitStatsTable[type]);
}

void Unit::SetStats(const UnitStats& s)
{
    m_stats = s;
    m_hp = m_stats.maxHp;
}

void Unit::StartAction()
{
    m_actionDone = false;
    m_phase = ActionPhase::None;

    // 애니메이션 상태 초기화
    m_rot.active = false;  m_rot.t = 0.0f; 
    m_move.active = false; m_move.t = 0.0f; m_move.soundPlayed = false;

    switch (m_action)
    {
    case TurnAction::Move:
        BeginRotateToDir();
        m_phase = ActionPhase::Rotating;
        break;

    case TurnAction::Attack:
        BeginRotateToDir();
        m_phase = ActionPhase::Rotating;
        break;

    case TurnAction::Miss:
        BeginRotateToDir();
        m_phase = ActionPhase::Rotating;
        break;

    case TurnAction::BreakWall:
        BeginRotateToDir();
        m_phase = ActionPhase::Rotating;
        break;

    default:
        m_phase = ActionPhase::None;
        m_actionDone = true;
        break;
    }
}

void Unit::UpdateAction(float dt)
{
    if (m_actionDone) return;
    if (dt <= 0.0f) return;

    switch (m_phase)
    {
    case ActionPhase::Rotating:
    {
        if (TickRotate(dt))
        {
            if (m_action == TurnAction::Move)
            {
                BeginMoveToPos();
                m_phase = ActionPhase::Moving;
                StartMoveAnim();
            }
            else 
            {
                StartAttackAnim();
                m_phase = ActionPhase::None;
                m_actionDone = true;
            }
        }
    } break;

    case ActionPhase::Moving:
    {
        if (TickMove(dt))
        {
            FinishAction();
        }
    } break;

    default:
        m_phase = ActionPhase::None;
        m_actionDone = true;
        break;
    }
}


int Unit::GetDir2(const Vector2& p) const // 대충 어느 위치 던져주면 그 위치를 바라보는 방향을 8방향으로 바꿔줌.
{
    float vx = p.x - m_pos.x;
    float vy = p.y - m_pos.y;

    if (vx == 0.0f && vy == 0.0f) return (int)m_dir; // 내 위치면 리턴.

    float ang = std::atan2(-vy, vx); // 각도 구하기. 위가 +y로 맞춰주기 위해서 -vy함.

    //Right=0, UpRight=1, Up=2, UpLeft=3, Left=4, DownLeft=5, Down=6, DownRight=7
    float step = 3.1415926535f / 4.0f; // 360 / 8 = 45도
    int dir = std::lround(ang / step); // 몇 번째 45도 칸?
    dir = (dir % 8 + 8) % 8; // 0 ~ 7 로 끼워넣기. 음수도.

    return dir;
}

void Unit::SetDir(const Vector2& p) 
{
    m_dir = static_cast<Dir8>(GetDir2(p));
}

void Unit::ResetTurnPlan()
{
    m_moveTarget = nullptr;
    m_moveTargetPos = GRIDPOS_INVALID;
    m_movePos = GRIDPOS_INVALID;
    if (!m_attackTarget || !m_attackTarget->IsAlive()) m_attackTarget = nullptr;
    m_attackPos = GRIDPOS_INVALID;
    m_action = TurnAction::Wait;
    if (m_isAlive) m_actionDone = false;
    else m_actionDone = true;
    m_animStart = false;
    m_isOnTrapTile = false;
    m_actionResultApplied = false;
}

Vector3 Unit::GridToWorld(const Vector2& p)
{
    return Vector3{ p.x * 2.0f, 1.0f, p.y * 2.0f };
}

float Unit::SmoothStep(float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

float Unit::SmoothStep2(float t, float start, float end)
{
    t = std::clamp(t, 0.0f, 1.0f);
    start = std::clamp(start, 0.0f, 1.0f);
    end = std::clamp(end, start + 0.0001f, 1.0f);

    if (t <= start) return 0.0f;
    if (t >= end)   return 1.0f;

    float u = (t - start) / (end - start);

    return u * u * (3.0f - 2.0f * u);
}

float Unit::LerpAngleDeg(float a, float b, float t)
{
    float d = DeltaAngleDeg(a, b);
    return NormalizeDeg(a + d * t);
}

float Unit::NormalizeDeg(float deg)
{
    float r = std::fmod(deg, 360.0f);
    if (r < 0.0f) r += 360.0f;
    return r;
}

float Unit::DirToYaw(Dir8 d)
{
    float yaw = (float)((int)d) * 45.0f - 90.0f;
    return NormalizeDeg(yaw);
}


float Unit::DeltaAngleDeg(float from, float to)
{
    float a = NormalizeDeg(from);
    float b = NormalizeDeg(to);
    float d = std::fmod(b - a + 540.0f, 360.0f) - 180.0f;
    return d;
}

void Unit::BeginRotateToDir()
{
    Vector3 euler = _GetOwner()->GetTransform()->GetEditorEuler();
    float curYaw = euler.y;

    m_rot.fromYaw = NormalizeDeg(curYaw);
    m_rot.toYaw = NormalizeDeg(DirToYaw(m_dir));
    m_rot.t = 0.0f;
    m_rot.active = true;
}

bool Unit::TickRotate(float dt)
{
    if (!m_rot.active) return true;

    auto* tr = _GetOwner()->GetTransform();
    Vector3 e = tr->GetEditorEuler();

    if (std::abs(DeltaAngleDeg(m_rot.fromYaw, m_rot.toYaw)) < 0.1f)
    {
        e.y = m_rot.toYaw;
        tr->SetRotationEuler(e);

        m_rot.active = false;
        return true;
    }

    float dur = (std::max)(m_rot.duration, 0.001f);
    m_rot.t += dt / dur;
    float t = (std::min)(m_rot.t, 1.0f);

    float s = SmoothStep(t);
    float yaw = LerpAngleDeg(m_rot.fromYaw, m_rot.toYaw, s);

    e.y = yaw;
    tr->SetRotationEuler(e);

    if (t >= 1.0f)
    {
        e.y = m_rot.toYaw;
        tr->SetRotationEuler(e);

        m_rot.active = false;
        return true;
    }

    return false;
}

void Unit::BeginMoveToPos()
{
    Transform* tr = _GetOwner()->GetTransform();

    m_move.from = tr->GetPosition();
    m_move.to = GridToWorld(m_pos);

    m_move.t = 0.0f;
    m_move.active = true;
}

bool Unit::TickMove(float dt)
{
    if (!m_move.active) return true;

    Transform* tr = _GetOwner()->GetTransform();

    float dur = (std::max)(m_move.duration, 0.001f);
    m_move.t += dt / dur;
    float t = (std::min)(m_move.t, 1.0f);

    if (!m_move.soundPlayed && t >= 0.75f)
    {
        SoundManager::Instance().PlayOneShot("SFX/Unit_Move(1)_ver.2");
        m_move.soundPlayed = true;
    }


    float s = SmoothStep2(t, 0.3f, 0.75f);
    Vector3 p = m_move.from + (m_move.to - m_move.from) * s;
    tr->SetPosition(p);

    if (t >= 1.0f)
    {
        tr->SetPosition(m_move.to);

        //SoundManager::Instance().PlayOneShot("SFX/Unit_Move(1)_ver.2");

        m_move.active = false;
        return true;
    }

    return false;
}

void Unit::FinishAction()
{
    Transform* tr = _GetOwner()->GetTransform();

    if (m_rot.active)
    {
        Vector3 e = tr->GetEditorEuler();
        e.y = m_rot.toYaw;
        tr->SetRotationEuler(e);
        m_rot.active = false;
    }

    if (m_move.active)
    {
        tr->SetPosition(m_move.to);
        m_move.active = false;
    }

    m_phase = ActionPhase::None;
    m_actionDone = true;
}

void Unit::PlayAnim(uint64_t id, uint64_t id1, uint64_t id2, float speed, bool loop)
{
    if (!m_anim) return;
    
    m_anim->SetClip(id);
    m_anim->SetTime(speed);
    m_anim->SetLoop(loop);

    if (id1 != ANIM_INVALID)
    {
        if (!m_anim1) return;
        m_anim1->SetClip(id1);
        m_anim1->SetTime(speed);
        m_anim1->SetLoop(loop);
    }

    if (id2 != ANIM_INVALID)
    {
        if (!m_anim2) return;
        m_anim2->SetClip(id2);
        m_anim2->SetTime(speed);
        m_anim2->SetLoop(loop);
    }


    m_anim->SetPlay(true);

    if (id1 != ANIM_INVALID)
    {
        m_anim1->SetPlay(true);
    }

    if (id2 != ANIM_INVALID)
    {
        m_anim2->SetPlay(true);
    }
}

void Unit::StartIdleAnim()
{
    if (!m_anim) return;
    uint64_t id = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/unit_Idle_1.fbx");
    uint64_t id1 = ANIM_INVALID;
    uint64_t id2 = ANIM_INVALID;
    if (m_weaponName == "Sword") {
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/sword_Idle.fbx");
        id2 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/shield_Idle.fbx");
    }
    else if (m_weaponName == "Spear") {
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/spear_Idle.fbx");
    }
    else if (m_weaponName == "Wand") {
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/wand_Idle.fbx");
    }
    PlayAnim(id, id1, id2, 1.0f, true);
}

void Unit::StartMoveAnim()
{
    if (!m_anim) return;
    if (m_animStart) return;
    uint64_t id = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/unit_move_1.fbx");
    uint64_t id1 = ANIM_INVALID;
    uint64_t id2 = ANIM_INVALID;
    if (m_weaponName == "Sword") {
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/sword_move.fbx");
        id2 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/shield_move.fbx");
    }
    else if (m_weaponName == "Spear") {
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/spear_move.fbx");
    }
    else if (m_weaponName == "Wand") {
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/wand_move.fbx");
    }
    //SoundManager::Instance().PlayOneShot("SFX/Unit_Move");
    PlayAnim(id, id1, id2, 1.0f, false);
    m_animStart = true;
}

void Unit::StartAttackAnim()
{
    if (!m_anim) return;
    if (m_animStart) return;
    uint64_t id = ANIM_INVALID;
    uint64_t id1 = ANIM_INVALID;
    uint64_t id2 = ANIM_INVALID;
    if (m_weaponName == "Sword") {
        id = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/unit_attack_sword_shield.fbx");
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/sword_attack.fbx");
        id2 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/shield_attack.fbx");

        SoundManager::Instance().PlayOneShot("SFX/Unit_Attack_Rook");
    }
    else if (m_weaponName == "Spear") {
        id = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/unit_attack_spear.fbx");
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/spear_attack.fbx");

        SoundManager::Instance().PlayOneShot("SFX/Unit_Attack_Knight");
    }
    else if (m_weaponName == "Wand") {
        id = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/unit_attack_wand.fbx");
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/wand_attack.fbx");

        SoundManager::Instance().PlayOneShot("SFX/Unit_Attack_Bishop");
    }
    PlayAnim(id, id1, id2, 1.0f, false);
    m_animStart = true;
}

void Unit::StartDieAnim()
{
    if (!m_anim) return;
    //if (m_animStart) return;
    //std::cout << _GetOwner()->_GetID() << std::endl;
    StartDissolve();

    uint64_t id = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/unit_die.fbx");
    uint64_t id1 = ANIM_INVALID;
    uint64_t id2 = ANIM_INVALID;
    if (m_weaponName == "Sword") {
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/sword_die.fbx");
        id2 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/shield_die.fbx");
    }
    else if (m_weaponName == "Spear") {
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/spear_die.fbx");
    }
    else if (m_weaponName == "Wand") {
        id1 = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Final_Rabbit/wand_die.fbx");
    }

    SoundManager::Instance().PlayOneShot("SFX/Unit_Death");
    PlayAnim(id, id1, id2, 1.0f, false);
    m_animStart = true;
}

void Unit::StartDissolve()
{


    auto tf = GetTransform();
    for (Transform* child : tf->GetChildren())
    {
        if (!child) continue;
        if (child->_GetOwner()->GetName() == "Chess")
        {

            auto cmp = child->_GetOwner()->AddComponent<Dissolved>();
            if (cmp)
            {
                auto cmp_e = child->_GetOwner()->AddComponent<Effect>();
                cmp->SetAlbedoTexture("Assets/Models/Final_Rabbit/Rabbit_Texture/Unit__BaseColor.png");
                cmp->SetNoiseTexture();
                cmp->InjectDissolveMaterila("Assets/Materials/Dissolve_MK_01.mat");

            }
        }
        else
        {
            auto cmp = child->_GetOwner()->AddComponent<Dissolved>();
            if (cmp)
            {
                auto cmp_e = child->_GetOwner()->AddComponent<Effect>();
                cmp->SetAlbedoTexture("Assets/Models/Final_Rabbit/weapon_low_01_BaseColor.png");
                cmp->SetNoiseTexture();
                cmp->InjectDissolveMaterila("Assets/Materials/Dissolve_MK_01.mat");

            }

        }
    }


   

    

}

