#include "RayCastHitEvent.h"
#include "GameObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "DX11Renderer.h"
#include "Camera.h"
#include "OpenPSEvent.h"
#include "OpenRSEvent.h"
#include "AllyUnit.h"
#include "EnemyUnit.h"



BEGINPROPERTY(RayCastHitEvent)
DTPROPERTY(RayCastHitEvent, m_rightPSWinodwBG)
DTPROPERTY(RayCastHitEvent, m_rightRSWinodwBG)
DTPROPERTY(RayCastHitEvent, m_leftPSWinodwBG)
DTPROPERTY(RayCastHitEvent, m_leftRSWinodwBG)
ENDPROPERTY()

void RayCastHitEvent::Update(float deltaTime)
{
		// 차피 밑에서 검사함.
		RaycastCheck();
}

void RayCastHitEvent::RaycastCheck()
{
		// 안켜져있으면 안할거임.
		if (!_GetOwner()->IsActive())
				return;

		auto& input = InputManager::Instance();
		Scene* scene = SceneManager::Instance().GetActiveScene();
		Camera* camera = scene->GetMainCamera();
		
		
		if (input.GetKeyDown(KeyCode::MouseLeft) && camera)
		{
				// 이거는 inputmanager에다가 받는거 추가해달라고 요청해야함. 아니면 가끔씩 터짐.
				const Vector2 mp = Vector2(input.GetGameMousePosition().x, input.GetGameMousePosition().y);
				std::cout << mp.x << " " << mp.y << std::endl;

				// 둘 중 한개라도 활성 상태고 마우스가 그 위에 있는지 체크.
				if (IsMouseInUI(m_leftPSWinodwBG, mp) || IsMouseInUI(m_leftRSWinodwBG, mp)
						|| IsMouseInUI(m_rightPSWinodwBG, mp) || IsMouseInUI(m_rightRSWinodwBG, mp))
				{
						return;
				}

				// 레이캐스트 시작.
				float x = mp.x;
				float y = mp.y;
				Ray r = camera->ScreenPointToRay(x, y);
				GameObject* hit = nullptr;
				float hitT = 0.0f;

				scene->Raycast2(r, hit, hitT);

				// 960 기준으로 좌우측 구분. 
				bool isLeft = x < 960;

				if (hit)
				{
						std::cout << hit->GetName() << std::endl;

						if (m_isHealSkillOn)
						{
								// ray timescale 상관없이 됨. 그냥 바로 작업하자.
								if (hit->GetName() == "Chess" || hit->GetName() == "Wand" || hit->GetName() == "Shield"
										|| hit->GetName() == "Sword" || hit->GetName() == "Spear" || hit->GetName() == "Shield")
								{
										GameObject* m_Unit = nullptr;
										GameObject* unitRoot = hit;
										if (hit->GetName().find("_Test") == std::string::npos) // 이름에 _Test가 없다면 자식으로 간주
										{
												if (hit->GetTransform()->GetParent())
														unitRoot = hit->GetTransform()->GetParent()->_GetOwner();
										}

										m_Unit = unitRoot;

										if (m_Unit)
										{
												auto unit = m_Unit->GetComponent<AllyUnit>();
												if (!unit)
														return;

												unit->Heal(30);
												m_isHealSkillOn = false;

												std::cout << unit->GetHp() << std::endl;
												SceneManager::Instance().GetActiveScene()->SetTimeScale(1);
										}
								}

								return;
						}

						if (m_isAttackSkillOn)
						{
								if (hit->GetName() == "Chess" || hit->GetName() == "Wand" || hit->GetName() == "Shield"
										|| hit->GetName() == "Sword" || hit->GetName() == "Spear" || hit->GetName() == "Shield")
								{
										GameObject* m_Unit = nullptr;
										GameObject* unitRoot = hit;
										if (hit->GetName().find("_Test") == std::string::npos) 
										{
												if (hit->GetTransform()->GetParent())
														unitRoot = hit->GetTransform()->GetParent()->_GetOwner();
										}

										m_Unit = unitRoot;

										if (m_Unit)
										{
												auto unit = m_Unit->GetComponent<EnemyUnit>();
												if (!unit)
														return;

												unit->TakeDamage(30);
												std::cout << unit->GetHp() << std::endl;
												m_isAttackSkillOn = false;

												SceneManager::Instance().GetActiveScene()->SetTimeScale(1);
										}
								}
								return;
						}

						// 전투 중 클릭해도 tile, chess 선택 안함. 위에는 스킬이라서 처리해야함.
						if (m_isStartBattle)
								return;

						if (hit->GetName() == "Height_01_White_Test" || hit->GetName() == "Height_01_Red_Test")
						{
								if (isLeft)
								{
										if (!m_leftPSWinodwBG)
												return;

										CloseAllWindows(); // 먼저 다 닫고
										auto psEvent = m_leftPSWinodwBG->GetComponent<OpenPSEvent>();
										psEvent->SetRayObj(hit);
										psEvent->SetActivePSWindow(); // PS 오픈
								}
								if (!isLeft)
								{
										if (!m_rightPSWinodwBG)
												return;

										CloseAllWindows(); // 먼저 다 닫고
										auto psEvent = m_rightPSWinodwBG->GetComponent<OpenPSEvent>();
										psEvent->SetRayObj(hit);
										psEvent->SetActivePSWindow(); // PS 오픈
								}
						}

						// RS 관련 오브젝트 클릭 시
						else if (hit->GetName() == "Chess" || hit->GetName() == "Wand" || hit->GetName() == "Shield" 
								|| hit->GetName() == "Sword" || hit->GetName() == "Spear" || hit->GetName() == "Shield")
						{ 
								if (isLeft)
								{
										if (!m_leftRSWinodwBG)
												return;

										CloseAllWindows(); // 먼저 다 닫고
										m_leftRSWinodwBG->GetComponent<OpenRSEvent>()->RequestOpenWindow(hit); // RS 오픈
								}
								if (!isLeft)
								{
										if (!m_rightRSWinodwBG)
												return;

										CloseAllWindows(); // 먼저 다 닫고
										m_rightRSWinodwBG->GetComponent<OpenRSEvent>()->RequestOpenWindow(hit); // RS 오픈
								}
						}


				}
		}
}

// 마우스가 특정 ui 영역 안에 있는지 체크할려고.
bool RayCastHitEvent::IsMouseInUI(GameObject* uiObj, const Vector2& mousePos)
{
		if (!uiObj || !uiObj->IsActive()) return false;

		Transform* uiTf = uiObj->GetTransform();
		Vector3 pos = uiTf->GetPosition();
		Vector3 scale = uiTf->GetScale();
		
		// 중앙 기준.
		/*float minX = pos.x - (scale.x * 0.5f);
		float maxX = pos.x + (scale.x * 0.5f);
		float minY = pos.y - (scale.y * 0.5f);
		float maxY = pos.y + (scale.y * 0.5f);*/

		// 0,0 피벗 기준
		float minX = pos.x;
		float maxX = pos.x + scale.x;
		float minY = pos.y;
		float maxY = pos.y + scale.y;

		return (mousePos.x >= minX && mousePos.x <= maxX &&
				mousePos.y >= minY && mousePos.y <= maxY);
}

void RayCastHitEvent::CloseAllWindows()
{
		if (m_rightPSWinodwBG) m_rightPSWinodwBG->SetActive(false);
		if (m_rightRSWinodwBG) m_rightRSWinodwBG->SetActive(false);

		if (m_leftPSWinodwBG) m_leftPSWinodwBG->SetActive(false);
		if (m_leftRSWinodwBG) m_leftRSWinodwBG->SetActive(false);
}
