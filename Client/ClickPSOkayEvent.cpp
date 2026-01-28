#include "ClickPSOkayEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "SelectIndexEvent.h"
#include "TilemapGenerator.h"
#include "Prefab.h"
#include "OpenPSEvent.h"
#include "OpenRSEvent.h"
#include "TilemapData.h"


BEGINPROPERTY(ClickPSOkayEvent)
DTPROPERTY(ClickPSOkayEvent, m_button)
DTPROPERTY(ClickPSOkayEvent, m_mapData)
DTPROPERTY(ClickPSOkayEvent, m_knight)
DTPROPERTY(ClickPSOkayEvent, m_rook)
DTPROPERTY(ClickPSOkayEvent, m_bishop)
DTPROPERTY(ClickPSOkayEvent, m_RSWindow)
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
								std::cout << "model prefab 없다" << std::endl;

						GameObject* parentObj = _GetOwner()->GetTransform()->GetParent()->_GetOwner();

						int idx = parentObj->GetComponent<SelectIndexEvent>()->GetIndex();
						std::cout << "idx = " << idx << std::endl;
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
								parentObj->SetActive(false);
								// 상태 바꿔주기.
								parentObj->GetComponent<OpenPSEvent>()->SetIsOpen(false);
								return;
						}
						
						Transform* targetTf = parentObj->GetComponent<OpenPSEvent>()->GetRayObjTransform();
						if (!targetTf) 
						{
								std::cout << "[Error] Ray Target Transform is Null!" << std::endl;
								return;
						}
					
						Vector3 worldPos = targetTf->GetWorldPosition();
						worldPos.y += 1;
						GameObject* go = m_selectPrefab->Instantiate();
						go->GetTransform()->SetPosition(worldPos);
						go->GetTransform()->SetRotationEuler(Vector3(0.0f, 125.0f, 0.0f));
						
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
										rswindow->SetTargetUnit(go);
								}
						}
				});
}


