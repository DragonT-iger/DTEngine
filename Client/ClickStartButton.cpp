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
#include "TilemapGenerator.h"


BEGINPROPERTY(ClickStartButton)
DTPROPERTY(ClickStartButton, m_startButton)
DTPROPERTY(ClickStartButton, m_combatObj)
DTPROPERTY(ClickStartButton, m_rayObj)
DTPROPERTY(ClickStartButton, m_tileMapObj)
DTPROPERTY(ClickStartButton, m_warningWindow)
DTPROPERTY(ClickStartButton, m_settingWindow)

DTPROPERTY(ClickStartButton, m_leftPrefabWindowBG)
DTPROPERTY(ClickStartButton, m_rightPrefabWindowBG)
DTPROPERTY(ClickStartButton, m_leftRuleWindowBG)
DTPROPERTY(ClickStartButton, m_rightRuleWindowBG)
DTPROPERTY(ClickStartButton, m_tilemapGenerator)
ENDPROPERTY()


void ClickStartButton::Start()
{
		if (!m_startButton || !m_combatObj || !m_rayObj || !m_tileMapObj || !m_warningWindow || !m_settingWindow)
				return;

		if (!m_leftPrefabWindowBG || !m_rightPrefabWindowBG || !m_leftRuleWindowBG || !m_rightRuleWindowBG)
				return;

		// 이미지 컴포넌트 가져오고.
		m_startClickImg = _GetOwner()->GetComponent<Image>();

		// 이미지 캐싱해두기. 재사용할려고.
		if (m_startClickImg)
		{
				m_defaultStartTextureID = m_startClickImg->GetTextureID();
		}

		m_startButton->SetOnClick([this]()
				{
						// 창 켜져있는 경우 안받음.
						if (m_leftPrefabWindowBG->IsActive() || m_rightPrefabWindowBG->IsActive() || m_leftRuleWindowBG->IsActive() ||
								m_rightRuleWindowBG->IsActive())
								return;

						// 이미 시작한 경우 다시 눌리지 않기 위해서.
						if (isStart)
								return;

						auto rayobj = m_rayObj->GetComponent<RayCastHitEvent>();

						// 시작하면 초기화. 
						if (rayobj->GetStartBattle())
								return;

						int managerMoney = GameManager::Instance()->GetMoney();

						if (managerMoney >= m_cost)
						{
								// 만약 settingwindow 켜져있으면 끄기.
								if (m_settingWindow->IsActive())
										m_settingWindow->SetActive(false);

								auto rayobj = m_rayObj->GetComponent<RayCastHitEvent>();
								rayobj->CloseAllWindows(); // window들도 다 꺼주기.
								m_warningWindow->SetActive(true);		// warningwindow 켜주고 
								// start 다시 안눌리게 처리하기. 근데 만약 취소 혹은 esc 누르면 다시 isStart를 false로 바꿔줘야함
								rayobj->SetRay(false);		// ray만 꺼주기.
								isStart = true;											
								return;
						}

						StartGame();
				});
}

void ClickStartButton::StartGame()
{

		auto tileGen = m_tileMapObj->GetComponent<TilemapGenerator>();
		if (!tileGen) return;

		auto children = m_tileMapObj->GetTransform()->GetChildren();

		//std::cout << "children 찾아서 바꿔주기." << std::endl;
		for (auto childTf : children)
		{
				GameObject* tileObj = childTf->_GetOwner();
				if (tileObj->GetName() == "Glow_R_Height_01_Red" ||
						tileObj->GetName() == "Glow_R_Height_01_White")
				{
						Vector3 pos = childTf->GetPosition();

						// 좌표만 구해서 함수에 전달
						Vector2 gridPos(
								std::round(pos.x / 2.0f),
								std::round(pos.z / 2.0f)
						);

						tileGen->ReplaceTileAtGrid(gridPos);
				}
		}

		//// 전투 시작 시 타일들 전부 깜빡거리는거 꺼주기 위해서.
		//for (auto obj : m_tileMapObj->GetTransform()->GetChildren())
		//{
		//		if (obj->GetName() == "Glow_W_Height_01_Red_Test" || obj->GetName() == "Glow_W_Height_01_White_Test")
		//		{
		//				auto effect = obj->GetComponent<Effect>();
		//				if (effect)
		//				{
		//						effect->SetAnimating(false);
		//						effect->SetEdgeColor(Vector4(0, 0, 0, 1));
		//				}
		//		}
		//}

		auto rayobj = m_rayObj->GetComponent<RayCastHitEvent>();

		// 이미지 바꿔주기.
		m_startClickImg->SetTextureID(AssetDatabase::Instance().GetIDFromPath("Assets/Models/UI/Alice_UI/Battle_start_button_2.png"));
		m_combatObj->GetComponent<CombatController>()->Setup();
		rayobj->SetStartBattle(true);
		rayobj->SetRay(true);
		isStart = true;
		
}

void ClickStartButton::ResetForStage()
{
		// 다음 스테이지/재도전 진입 시 Start 버튼 상태를 기본으로 복구.
		isStart = false;

		if (m_startClickImg && m_defaultStartTextureID != 0)
		{
				m_startClickImg->SetTextureID(m_defaultStartTextureID);
		}
}
