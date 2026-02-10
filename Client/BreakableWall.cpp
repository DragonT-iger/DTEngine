#include "BreakableWall.h"
#include "AssetDatabase.h"
#include "GameObject.h"
#include "Animatior.h"
#include "TilemapGenerator.h"
#include "Dissolved.h"
#include "Effect.h"

BEGINPROPERTY(BreakableWall)
ENDPROPERTY()

void BreakableWall::Update(float dTime)
{
	if (!m_animStart || m_animEnd) return;

	if (m_anim->GetAnimationDone())
	{
		m_animEnd = true;

		if (m_tilemapGenerator)
		{
			m_tilemapGenerator->ReplaceTileAtGrid(m_gridPos);
		}
	}
}

void BreakableWall::Init(TilemapGenerator* tg, const Vector2& gridPos)
{
	m_tilemapGenerator = tg;
	m_gridPos = gridPos;
	m_anim = _GetOwner()->GetComponent<Animator>();
	if (m_anim)
	{
		m_anim->SetLoop(false);
		m_anim->SetPlay(false);
	}
}

void BreakableWall::StartWallBreakAnim()
{
	if (m_animStart) return;

	if (!m_anim) 
	{
		m_animEnd = true;
		if (m_tilemapGenerator) m_tilemapGenerator->ReplaceTileAtGrid(m_gridPos);
		return;
	}



	//auto cmp = this->_GetOwner()->AddComponent<Dissolved>();
	//if (cmp)
	//{
	//	auto cmp_e = this->_GetOwner()->AddComponent<Effect>();
	//	cmp->SetAlbedoTexture("Assets/Models/BackGround/Height_Tile/Height_BaseColor.png");
	//	cmp->SetNoiseTexture();
	//	cmp_e->SetTimeMultiplier(0.1f);
	//	cmp->InjectDissolveMaterila("Assets/Materials/Dissolve_MK_01.mat");

	//}



	m_anim->SetTime(1.0f);
	m_anim->SetLoop(false);
	m_anim->SetPlay(true);

	m_animStart = true;
}
