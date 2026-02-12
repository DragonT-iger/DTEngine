#include "ClickPSOkayEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "SelectIndexEvent.h"
#include "TilemapGenerator.h"
#include "Prefab.h"
#include "OpenPSEvent.h"
#include "OpenRSEvent.h"
#include "TilemapData.h"
#include "AllyUnit.h"
#include "CombatController.h"
#include "Unit.h"
#include "EnemyUnit.h"
#include "HPBarFollowEvent.h"
#include "SceneManager.h"
#include "Scene.h"
#include "TutorialManager.h"
#include "SoundManager.h"

#include "Effect.h"
#include "GameManager.h"

BEGINPROPERTY(ClickPSOkayEvent)
DTPROPERTY(ClickPSOkayEvent, m_nonCostObj)
DTPROPERTY(ClickPSOkayEvent, m_button)
//DTPROPERTY(ClickPSOkayEvent, m_mapData)
DTPROPERTY(ClickPSOkayEvent, m_knight)
DTPROPERTY(ClickPSOkayEvent, m_rook)
DTPROPERTY(ClickPSOkayEvent, m_bishop)
DTPROPERTY(ClickPSOkayEvent, m_RSWindow)
DTPROPERTY(ClickPSOkayEvent, m_combatObj)
DTPROPERTY(ClickPSOkayEvent, m_tutorialManager)
DTPROPERTY(ClickPSOkayEvent, m_tilemapGenerator)
ENDPROPERTY()

void ClickPSOkayEvent::Start()
{
		SetClick();
}

void ClickPSOkayEvent::SetClick()
{
		if (!m_button)
				return;

		m_button->SetOnClick([this]() 
				{
						//if (!m_mapData)
						//		return;

						if (!m_knight || !m_rook || !m_bishop)
								return;
								//std::cout << "model prefab 없다" << std::endl;

						int managerMoney = GameManager::Instance()->GetMoney();

						if (m_cost > managerMoney)
						{
								if (m_nonCostObj)
								{
										m_nonCostObj->SetActive(true);
								}
								//std::cout << "cost 초과." << std::endl;
								return;
						}
						
						GameObject* parentObj = _GetOwner()->GetTransform()->GetParent()->_GetOwner();

						int idx = parentObj->GetComponent<SelectIndexEvent>()->GetIndex();
						switch (idx)
						{
						case 1:
								m_selectPrefab = m_knight; 
								break;
						case 2:
								m_selectPrefab = m_rook;
								break;
						case 3:
								m_selectPrefab = m_bishop;
								break;
						default:
								m_selectPrefab = nullptr;
								break;
						}
						
						if (!m_selectPrefab)
						{
							if (idx == 0 && SceneManager::Instance().GetActiveScene()->GetName() == "TutorialScene")
							{
								return;
							}

							parentObj->SetActive(false);
							parentObj->GetComponent<OpenPSEvent>()->SetIsOpen(false);
							return;
						}
						
						Transform* targetTf = parentObj->GetComponent<OpenPSEvent>()->GetRayObjTransform();
						Vector3 worldPos = targetTf->GetPosition();
						worldPos.y += 1;
						GameObject* go = m_selectPrefab->Instantiate();
						go->GetTransform()->SetRotationEuler(Vector3(0.0f, 180.0f, 0.0f));
						go->GetTransform()->SetPosition(worldPos);
						
						SoundManager::Instance().PlayOneShot("SFX/Unit_Place");


						//std::cout << worldPos.x << " " << worldPos.y << std::endl;

						// allyunit 컴포넌트 가져와서 setpos 추가해주기.
						auto allyC = go->GetComponent<AllyUnit>();
						if (allyC)
						{
								Vector2 pos = Vector2((worldPos.x / 2), (worldPos.z / 2));
								
								allyC->SetPos(pos);

								if (m_combatObj)
								{
										m_combatObj->GetComponent<CombatController>()->AddAllyUnit(allyC);
								}

								// ally 리스트에 추가해주기. 
								//if (GameManager::Instance())
								//{
								//		GameManager::Instance()->RegisterRuntimeAlly(go);
								//}
						}
						
						// 상태 바꿔주기.
						parentObj->GetComponent<OpenPSEvent>()->SetIsOpen(false);

						// iCube로 바꿔서 다시 rayhit 안되게 하기. | 이제 tilemapgenerator에 replace 사용하기.
						OpenPSEvent* openEvent = parentObj->GetComponent<OpenPSEvent>();
						GameObject* targetObj = openEvent->GetRayObj();

						if (m_tilemapGenerator)
						{
								auto tileGen = m_tilemapGenerator->GetComponent<TilemapGenerator>();

								if (targetObj && m_tilemapGenerator)
								{
										Vector3 pos = targetObj->GetTransform()->GetPosition();

										// 좌표만 구해서 함수에 전달
										Vector2 gridPos(
												std::round(pos.x / 2.0f),
												std::round(pos.z / 2.0f)
										);

										tileGen->ReplaceTileAtGrid(gridPos);

										/*auto effect = targetObj->GetComponent<Effect>();
										if (effect)
										{
												effect->SetAnimating(false);
												effect->SetEdgeColor(Vector4(0, 0, 0, 1));
										}

										targetObj->SetName("iCube");*/
								}
						}
						
						// 창 닫기.
						parentObj->SetActive(false);
						openEvent->SetIsOpen(false);

						if (SceneManager::Instance().GetActiveScene()->GetName() == "TutorialScene") {
							if (m_tutorialManager) m_tutorialManager->NextStep(true);
							return;
						}

						// 생성 성공이니까 manager unit count 증가시켜주기.
						GameManager::Instance()->SetMoney(managerMoney - m_cost);
						if (m_RSWindow)
						{
								auto rswindow = m_RSWindow->GetComponent<OpenRSEvent>();
								if (rswindow) 
								{
										rswindow->RequestOpenWindow(go);
								}
						}

						
				});
}
