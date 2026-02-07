#include "RedQueen.h"
#include "GameObject.h"
#include "AssetDatabase.h"
#include "Dissolved.h"
#include "Effect.h"

BEGINPROPERTY(RedQueenUnit)
DTPROPERTY_ACCESSOR(RedQueenUnit, m_pos, GetPos, SetPos)
ENDPROPERTY()

RedQueenUnit::RedQueenUnit()
{
	SetTeam(Team::Enemy);
	SetUnitType(UnitType::Queen);
}

void RedQueenUnit::StartQueenDieAnim()
{
    if (!m_anim) return;
    //if (m_animStart) return;
    //std::cout << _GetOwner()->_GetID() << std::endl;
    //StartDissolve();

    uint64_t id = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Main_Char/queen_Die.fbx");
    PlayAnim(id, ANIM_INVALID, ANIM_INVALID, 1.0f, false);
    m_animStart = true;
}