#include "AliceUnit.h"
#include "GameObject.h"
#include "AssetDatabase.h"
#include "Dissolved.h"
#include "Effect.h"

BEGINPROPERTY(AliceUnit)
DTPROPERTY_ACCESSOR(AliceUnit, m_pos, GetPos, SetPos)
ENDPROPERTY()

AliceUnit::AliceUnit()
{
	SetTeam(Team::Ally);
	SetUnitType(UnitType::Alice);
}

void AliceUnit::StartAliceDieAnim()
{
    if (!m_anim) return;
    //if (m_animStart) return;
    //std::cout << _GetOwner()->_GetID() << std::endl;
    //StartDissolve();

    uint64_t id = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Main_Char/alice_Die.fbx");
    PlayAnim(id, ANIM_INVALID, ANIM_INVALID, 1.0f, false);
    m_animStart = true;
}