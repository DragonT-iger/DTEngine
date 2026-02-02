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


BEGINPROPERTY(ClickPSOkayEvent)
DTPROPERTY(ClickPSOkayEvent, m_button)
DTPROPERTY(ClickPSOkayEvent, m_mapData)
DTPROPERTY(ClickPSOkayEvent, m_knight)
DTPROPERTY(ClickPSOkayEvent, m_rook)
DTPROPERTY(ClickPSOkayEvent, m_bishop)
DTPROPERTY(ClickPSOkayEvent, m_RSWindow)
DTPROPERTY(ClickPSOkayEvent, m_combatObj)
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
						if (!m_mapData)
								return;

						if (!m_knight || !m_rook || !m_bishop)
								return;
								//std::cout << "model prefab 없다" << std::endl;

						GameObject* parentObj = _GetOwner()->GetTransform()->GetParent()->_GetOwner();

						int idx = parentObj->GetComponent<SelectIndexEvent>()->GetIndex();
						switch (idx)
						{
						case 1:
								m_selectPrefab = m_rook;
								break;
						case 2:
								m_selectPrefab = m_knight;
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
								parentObj->SetActive(false);
								// 상태 바꿔주기.
								parentObj->GetComponent<OpenPSEvent>()->SetIsOpen(false);
								return;
						}
						
						Transform* targetTf = parentObj->GetComponent<OpenPSEvent>()->GetRayObjTransform();
						Vector3 worldPos = targetTf->GetPosition();
						worldPos.y += 1;
						GameObject* go = m_selectPrefab->Instantiate();
						go->GetTransform()->SetRotationEuler(Vector3(0.0f, 90.0f, 0.0f));
						go->GetTransform()->SetPosition(worldPos);
						
						//std::cout << worldPos.x << " " << worldPos.y << std::endl;

						// allyunit 컴포넌트 가져와서 setpos 추가해주기.
						auto allyC = go->GetComponent<AllyUnit>();
						if (allyC)
						{
								Vector2 pos = Vector2((worldPos.x / 2), (worldPos.z / 2));
								
								allyC->SetPos(pos);

								//std::cout << pos.x << " " << pos.y << std::endl;
								if (m_combatObj)
								{
										switch (index)
										{
										case 0:
												m_combatObj->GetComponent<CombatController>()->SetAllyUnit0(allyC);
												break;
										case 1:
												m_combatObj->GetComponent<CombatController>()->SetAllyUnit1(allyC);
												break;
										case 2:
												m_combatObj->GetComponent<CombatController>()->SetAllyUnit2(allyC);
												break;
										default:
												index = 100;
												break;
										}
										index++;
								}
						}

						// 상태 바꿔주기.
						parentObj->GetComponent<OpenPSEvent>()->SetIsOpen(false);

						// iCube로 바꿔서 다시 rayhit 안되게 하기.
						OpenPSEvent* openEvent = parentObj->GetComponent<OpenPSEvent>();
						GameObject* targetObj = openEvent->GetRayObj();

						if (targetObj)
						{
								targetObj->SetName("iCube");
						}
						// 창 닫기.
						parentObj->SetActive(false);
						openEvent->SetIsOpen(false);

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
