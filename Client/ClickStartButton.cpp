#include "ClickStartButton.h"
#include "UIButton.h"
#include "AllyUnit.h"
#include "GameObject.h"
#include "CombatController.h"
#include "RayCastHitEvent.h"
#include "Effect.h"
#include "AssetDatabase.h"
#include "Image.h"
#include "GameManager.h"

BEGINPROPERTY(ClickStartButton)
DTPROPERTY(ClickStartButton, m_startButton)
DTPROPERTY(ClickStartButton, m_combatObj)
DTPROPERTY(ClickStartButton, m_rayObj)
DTPROPERTY(ClickStartButton, m_tileMapObj)
ENDPROPERTY()


void ClickStartButton::Start()
{
		if (!m_startButton || !m_combatObj || !m_rayObj || !m_tileMapObj)
				return;

		// 이미지 컴포넌트 가져오고.
		m_startClickImg = _GetOwner()->GetComponent<Image>();

		m_startButton->SetOnClick([this]()
				{

						int managerUnitCount = GameManager::Instance()->GetUnitCount();
						int managerMoney = GameManager::Instance()->GetMoney();

						// 이후에는 gameobject 한개 가지고 있어서 띄우기.
						if (managerUnitCount < m_maxCount && managerMoney >= m_cost)
						{
								std::cout << "유닛 생성 가능한 count 및 cost 보유." << std::endl;
								return;
						}

						// 전투 시작 시 타일들 전부 깜빡거리는거 꺼주기 위해서.
						for (auto obj : m_tileMapObj->GetTransform()->GetChildren())
						{
								if (obj->GetName() == "Glow_W_Height_01_Red_Test" || obj->GetName() == "Glow_W_Height_01_White_Test")
								{
										auto effect = obj->GetComponent<Effect>();
										if (effect)
										{
												effect->SetAnimating(false);
												effect->SetEdgeColor(Vector4(0, 0, 0, 1));
										}
								}
						}
						
						// 이미지 바꿔주기.
						m_startClickImg->SetTextureID(AssetDatabase::Instance().GetIDFromPath("Assets/Models/UI/Alice_UI/Battle_start_button_2.png"));
						m_combatObj->GetComponent<CombatController>()->Setup();
						m_rayObj->GetComponent<RayCastHitEvent>()->SetStartBattle(true);
				});
}


