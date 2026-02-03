#include "RayCastHitEvent.h"
#include "GameObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "DX11Renderer.h"
#include "Camera.h"
#include "OpenPSEvent.h"
#include "OpenRSEvent.h"
#include "TutorialManager.h"
#include "AllyUnit.h"
#include "EnemyUnit.h"
#include "ArrowObjectPool.h"


BEGINPROPERTY(RayCastHitEvent)
DTPROPERTY(RayCastHitEvent, m_rightPSWinodwBG)
DTPROPERTY(RayCastHitEvent, m_rightRSWinodwBG)
DTPROPERTY(RayCastHitEvent, m_leftPSWinodwBG)
DTPROPERTY(RayCastHitEvent, m_leftRSWinodwBG)
DTPROPERTY(RayCastHitEvent, m_arrowPool)

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

		if (m_rightPSWinodwBG->IsActive() || m_leftPSWinodwBG->IsActive()) 
		{
			return;
		}

		if (m_rightRSWinodwBG->IsActive() || m_leftRSWinodwBG->IsActive()) 
		{
			return;
		}

		// UI 켜져있을때 ray 검사 안함


		auto& input = InputManager::Instance();
		Scene* scene = SceneManager::Instance().GetActiveScene();
		Camera* camera = scene->GetMainCamera();


		// 튜토리얼 씬인 경우에 예외처리



		if (scene->GetName() == "TutorialScene") {
			if (m_tutorialManager == nullptr) return;
			if (!m_tutorialManager->GetRayActive()) return;
		}


		if (input.GetKeyDown(KeyCode::MouseLeft) && camera)
		{
				const Vector2 mp = Vector2((float)input.GetGameMousePosition().x, (float)input.GetGameMousePosition().y);

#ifdef _DEBUG
				float screenWidth = (float)input.GetGameResolution().x;
#else
				float screenWidth = DX11Renderer::Instance().GetWidth();
#endif

				float screenCenter = screenWidth * 0.5f;

				bool isLeft = mp.x > screenCenter;

				//if (IsMouseInUI(m_leftPSWinodwBG, mp) || IsMouseInUI(m_leftRSWinodwBG, mp)
				//		|| IsMouseInUI(m_rightPSWinodwBG, mp) || IsMouseInUI(m_rightRSWinodwBG, mp))
				//{
				//		return;
				//} 그냥 UI 키고 끌때 예외처리 하는게 나을듯

				// 레이캐스트 시작.
				float x = mp.x;
				float y = mp.y;
				Ray r = camera->ScreenPointToRay(x, y);
				GameObject* hit = nullptr;
				float hitT = 0.0f;

				scene->Raycast2(r, hit, hitT);

				//bool isLeft = x < 960;

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
										if (hit->GetName().find("_V1") == std::string::npos) // 이름에 _Test가 없다면 자식으로 간주
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
										if (hit->GetName().find("_V1") == std::string::npos) 
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

						// 여기는 화살표 보여주기 위해서
						if (CheckEnemyObj(hit))
						{
								// 여기가 적인 경우임. 그래서 m_Unit이 GameObject 타입이고 unit은 enemyunit component여서 여기서 처리해주면 됨.
								// 3개의 경로를 전부 다 가지고 있고 선택 시 각각 다른애들 다 끄고 본인만 켜지게 하는 로직이 있어야겠네. 
								// prefab이 있다보니까 결국 걍 prefab으로 생성을 시키고 전부 setactive false를 하면 다 꺼지잖아.
								// 클릭되는 경우 해당하는 prefab으로 만들어둔 object setacitve true 나머지 다 false하면 되는거니까
								// level design이 먼저라 생각하는데 배치 위치에 따라 우리가 작업을 모든 예외처리를 다 할 필요가 없다 라는 생각이라.
						}

						if (hit->GetName() == "Glow_W_Height_01_Red_Test" || hit->GetName() == "Glow_W_Height_01_White_Test")
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

bool RayCastHitEvent::CheckEnemyObj(GameObject* obj)
{
		if (obj->GetName() == "Chess" || obj->GetName() == "Wand" || obj->GetName() == "Shield"
				|| obj->GetName() == "Sword" || obj->GetName() == "Spear" || obj->GetName() == "Shield")
		{
				GameObject* m_Unit = nullptr;
				GameObject* unitRoot = obj;
				if (obj->GetName().find("_V1") == std::string::npos)
				{
						if (obj->GetTransform()->GetParent())
								unitRoot = obj->GetTransform()->GetParent()->_GetOwner();
				}

				m_Unit = unitRoot;

				if (m_Unit)
				{
						auto unit = m_Unit->GetComponent<EnemyUnit>();
						if (!unit)
								return false;

						return true;
				}
		}

		return false;
}
