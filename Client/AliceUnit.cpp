#include "AliceUnit.h"
#include "GameObject.h"
#include "AssetDatabase.h"
#include "Dissolved.h"
#include "Effect.h"
#include "SoundManager.h"

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
    if (!m_anim)
    {
        //std::cout << "????????????왜없음\n";
        return;
    }
    //if (m_animStart) return;
    //std::cout << _GetOwner()->_GetID() << std::endl;
    //StartDissolve();

    auto cmp = this->_GetOwner()->AddComponent<Dissolved>();
    if (cmp)
    {
        auto cmp_e = this->_GetOwner()->AddComponent<Effect>();
        cmp->SetAlbedoTexture("Assets/Models/Main_Char/Alice_BaseColor.png");
        cmp->SetNoiseTexture();
        cmp->InjectDissolveMaterila("Assets/Materials/Dissolve_MK_01.mat");

    }

    uint64_t id = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Main_Char/alice_Die.fbx");
    //if (id == 0) std::cout << "????????????\n";
    SoundManager::Instance().PlayOneShot("SFX/Unit_Death");
    PlayAnim(id, ANIM_INVALID, ANIM_INVALID, 1.0f, false);
    m_animStart = true;
}