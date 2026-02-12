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
#include "GameManager.h"
#include "ClickStartButton.h"
#include "SkillButton.h"
#include "UIButton.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "Text.h"

BEGINPROPERTY(RayCastHitEvent)
DTPROPERTY(RayCastHitEvent, m_settingWindowBG)
DTPROPERTY(RayCastHitEvent, m_rightPSWinodwBG)
DTPROPERTY(RayCastHitEvent, m_rightRSWinodwBG)
DTPROPERTY(RayCastHitEvent, m_leftPSWinodwBG)
DTPROPERTY(RayCastHitEvent, m_leftRSWinodwBG)
DTPROPERTY(RayCastHitEvent , m_tutorialManager)
DTPROPERTY(RayCastHitEvent, m_arrowPool)
DTPROPERTY(RayCastHitEvent, m_warningWindowBG)
DTPROPERTY(RayCastHitEvent, m_startButton)
DTPROPERTY(RayCastHitEvent, m_skillButton)
DTPROPERTY(RayCastHitEvent, m_attackCountText)
DTPROPERTY(RayCastHitEvent, m_healCountText)
ENDPROPERTY()

void RayCastHitEvent::Update(float deltaTime)
{
		// 차피 밑에서 검사함.
		RaycastCheck();
		auto& input = InputManager::Instance();
		if (input.GetKeyDown(KeyCode::Escape))
				ToggleSettingWindow();
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

		if (scene->GetName() == "EndingScene") {
			return;
		}

		if (scene->GetName() == "TutorialScene") {
			if (m_tutorialManager == nullptr) return;
			if (!m_tutorialManager->GetRayActive()) return;
		}


		if (m_isStartBattle && m_lastSelectedEnemy)
		{
			m_lastSelectedEnemy = nullptr;
			m_arrowPool->_GetOwner()->SetActive(false);
		}

		if (input.GetKeyDown(KeyCode::MouseLeft) && camera && m_isRay)
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

					if (m_tutorialManager &&
						m_tutorialManager->GetCurrentStep() >= TutorialManager::TutorialStep::Cat_Explain_EnemyPath &&
						m_tutorialManager->GetCurrentStep() < TutorialManager::TutorialStep::Cat_Explain_EnemyPathWait)
					{
						bool isEnemy = (hit->GetTag() == "Enemy");
						if (!isEnemy && hit->GetTransform()->GetParent() && hit->GetTransform()->GetParent()->_GetOwner()->GetTag() == "Enemy")
						{
							isEnemy = true;
						}

						if (isEnemy)
						{
 							EnemyUnit* enemy = hit->GetComponent<EnemyUnit>();
							if (!enemy && hit->GetTransform()->GetParent())
							{
								enemy = hit->GetTransform()->GetParent()->_GetOwner()->GetComponent<EnemyUnit>();
							}
							if (!enemy) enemy = CheckEnemyObj(hit); 

							if (enemy && m_arrowPool)
							{
								m_arrowPool->_GetOwner()->SetActive(false); 

								std::vector<ArrowSegment> path = enemy->GetArrowSegments();
								std::vector<GameObject*> heads = m_arrowPool->GetHeads();
								std::vector<GameObject*> bodys = m_arrowPool->GetBodys();
								//std::vector<GameObject*> starts = m_arrowPool->GetStarts();

								m_arrowPool->DeactivateAll();

								// 화살표 배치
								for (int i = 0; i < path.size() - 1; ++i) // magic -1
								{
									Transform* tr = bodys[i]->GetTransform();
									tr->SetPosition(path[i].midWorld);
									Vector3 s = tr->GetScale();
									s.x = path[i].lenWorld;
									tr->SetScale(s);
									Vector3 e = tr->GetEditorEuler();
									e.y = path[i].yawDeg;
									tr->SetRotationEuler(e);
									bodys[i]->SetActive(true);

									tr = heads[i]->GetTransform();
									tr->SetPosition(path[i].headWorld);
									e = tr->GetEditorEuler();
									e.y = path[i].yawDeg;
									tr->SetRotationEuler(e);
									heads[i]->SetActive(true);

									//tr = starts[i]->GetTransform();
									//tr->SetPosition(path[i].startWorld);
									//e = tr->GetEditorEuler();
									//e.y = path[i].yawDeg;
									//tr->SetRotationEuler(e);
									//starts[i]->SetActive(true);
								}
								m_arrowPool->_GetOwner()->SetActive(true);

								m_tutorialManager->NextStep(true);
								// 중요: 여기서 return 하여 아래쪽의 RS 창 로직이 실행되지 않도록 함
								return;
							}
						}

						// 적이 아닌 것을 클릭했거나, EnemyUnit을 못 찾은 경우 클릭 무시
						return;
					}
						std::cout << hit->GetName() << std::endl;
						if (scene->GetName() == "TutorialScene") {
							if (hit->GetName() == "Glow_R_Height_01_White" && hit->GetComponent<Transform>()->GetPosition().z < 9) {
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

								if(m_tutorialManager) m_tutorialManager->NextStep(true);
							}
							else if (hit->GetName() == "Chess") {
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
								if (m_tutorialManager) m_tutorialManager->NextStep(true);
							}

							else {
								return;
							}
						}

						//std::cout << hit->GetName() << std::endl;

						if (m_isHealSkillOn && m_isStartBattle)	// 배틀 상태에서만.
						{
								// ray timescale 상관없이 됨. 그냥 바로 작업하자.
								if (hit->GetName() == "Chess" || hit->GetName() == "Wand" || hit->GetName() == "Shield"
										|| hit->GetName() == "Sword" || hit->GetName() == "Spear")
								{
										GameObject* m_Unit = nullptr;
										GameObject* unitRoot = hit;
										if (hit->GetName().find("_V1") == std::string::npos) // 이름에 _Test가 없다면 자식으로 간주
										{
												if (hit->GetTransform()->GetParent())
														unitRoot = hit->GetTransform()->GetParent()->_GetOwner();
										}

										m_Unit = unitRoot;

										int healCount = GameManager::Instance()->GetHealSkillcount();
										if (m_Unit)
										{
												auto unit = m_Unit->GetComponent<AllyUnit>();
												if (!unit)
														return;

												unit->Heal(30);
												SoundManager::Instance().PlayOneShot("SFX/Skill_Heal");
												m_isHealSkillOn = false;

												
												int prevScale = GameManager::Instance()->GetPrevTimeScale();
												GameManager::Instance()->SetTimeScale(prevScale);

												// skill count 감소 시키기.
												GameManager::Instance()->SetHealSkillCount(healCount - 1);
												m_healCountText->SetText(std::to_wstring(healCount - 1));
										}
								}

								return;
						}

						if (m_isAttackSkillOn && m_isStartBattle) // 배틀 상태에서만.
						{
								if (hit->GetName() == "Chess" || hit->GetName() == "Wand" || hit->GetName() == "Shield"
										|| hit->GetName() == "Sword" || hit->GetName() == "Spear")
								{
										GameObject* m_Unit = nullptr;
										GameObject* unitRoot = hit;
										if (hit->GetName().find("_V1") == std::string::npos) 
										{
												if (hit->GetTransform()->GetParent())
														unitRoot = hit->GetTransform()->GetParent()->_GetOwner();
										}

										m_Unit = unitRoot;

										int attackCount = GameManager::Instance()->GetAttackSkillcount();
										if (m_Unit)
										{
												auto unit = m_Unit->GetComponent<EnemyUnit>();
												if (!unit)
														return;
												EffectManager::Instance().PlayEffect("Poison", unit->_GetOwner());
												unit->TakeDamage(20);
												SoundManager::Instance().PlayOneShot("SFX/Skill_Poison");
												std::cout << unit->GetHp() << std::endl;
												m_isAttackSkillOn = false;

												int prevScale = GameManager::Instance()->GetPrevTimeScale();
												GameManager::Instance()->SetTimeScale(prevScale);

												// skill count 감소 시키기.
												GameManager::Instance()->SetAttackSkillCount(attackCount - 1);
												m_attackCountText->SetText(std::to_wstring(attackCount - 1));
										}
								}
								return;
						}

						
					
						// 전투 중 클릭해도 tile, chess 선택 안함. 위에는 스킬이라서 처리해야함.
						if (m_isStartBattle)
						{
							return;
						}


						// 여기는 화살표 보여주기 위해서
						EnemyUnit* enemy = CheckEnemyObj(hit);

						if (enemy)
						{
								// 여기가 적인 경우임. 그래서 m_Unit이 GameObject 타입이고 unit은 enemyunit component여서 여기서 처리해주면 됨.
								// 3개의 경로를 전부 다 가지고 있고 선택 시 각각 다른애들 다 끄고 본인만 켜지게 하는 로직이 있어야겠네. 
								// prefab이 있다보니까 결국 걍 prefab으로 생성을 시키고 전부 setactive false를 하면 다 꺼지잖아.
								// 클릭되는 경우 해당하는 prefab으로 만들어둔 object setacitve true 나머지 다 false하면 되는거니까
								// level design이 먼저라 생각하는데 배치 위치에 따라 우리가 작업을 모든 예외처리를 다 할 필요가 없다 라는 생각이라.
							if (!m_arrowPool) return;

							SoundManager::Instance().PlayOneShot("SFX/UI_Click_ver.2");

							if (enemy == m_lastSelectedEnemy)
							{
								m_lastSelectedEnemy = nullptr;
								m_arrowPool->_GetOwner()->SetActive(false);

								return;
							}

							m_lastSelectedEnemy = enemy;

							m_arrowPool->_GetOwner()->SetActive(false);

							std::vector<ArrowSegment> path = enemy->GetArrowSegments();
							std::vector<GameObject*> heads = m_arrowPool->GetHeads();
							std::vector<GameObject*> bodys = m_arrowPool->GetBodys();
							//std::vector<GameObject*> starts = m_arrowPool->GetStarts();

							m_arrowPool->DeactivateAll();

							for (int i = 0; i < path.size(); ++i)
							{
								Transform* tr = bodys[i]->GetTransform();
								tr->SetPosition(path[i].midWorld);
								Vector3 s = tr->GetScale();
								s.x = path[i].lenWorld;
								tr->SetScale(s);
								Vector3 e = tr->GetEditorEuler();
								e.y = path[i].yawDeg;
								tr->SetRotationEuler(e);
								bodys[i]->SetActive(true);

								tr = heads[i]->GetTransform();
								tr->SetPosition(path[i].headWorld);
								e = tr->GetEditorEuler();
								e.y = path[i].yawDeg;
								tr->SetRotationEuler(e);
								heads[i]->SetActive(true);

								//tr = starts[i]->GetTransform();
								//tr->SetPosition(path[i].startWorld);
								//e = tr->GetEditorEuler();
								//e.y = path[i].yawDeg;
								//tr->SetRotationEuler(e);
								//starts[i]->SetActive(true);
							}

							m_arrowPool->_GetOwner()->SetActive(true);
							

							return;
						}

						if (hit->GetName() == "Glow_R_Height_01_White" || hit->GetName() == "Glow_R_Height_01_Red")
						{
							SoundManager::Instance().PlayOneShot("SFX/UI_Click_ver.2");
							
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

void RayCastHitEvent::ToggleSettingWindow()
{
		if (!m_settingWindowBG || !m_warningWindowBG || !m_startButton)
				return;

		if (m_finishGame)
		{
				std::cout << "게임끝남" << std::endl;
				return;
		}
				
		if (m_isAttackSkillOn)
		{
				m_isAttackSkillOn = false;	
		}
		if (m_isHealSkillOn)
		{
				m_isHealSkillOn = false;
		}

		if (m_warningWindowBG->IsActive())
		{
				// warningwindow 꺼주고. isStart 다시 set 해주기.
				m_warningWindowBG->SetActive(false);

				if (!m_isStartBattle)
						m_startButton->GetComponent<ClickStartButton>()->SetIsStart(false);
		}
		ApplySettingWindow();
}

void RayCastHitEvent::ApplySettingWindow()
{
		// toggle 해주기.
		CloseAllWindows();
		m_settingWindowBG->SetActive(!m_settingWindowBG->IsActive());

		// ray 역시 조절, timescale 조절, start button 막기.
		if (m_settingWindowBG->IsActive()) // 설정창 열림
		{
				GameManager::Instance()->SetTimeScale(0.0f);
				SetRay(false);
				SetButtons(false);
		}
		else // 설정창 닫힘.
		{
				// 이전 배속으로 복원
				int prevScale = GameManager::Instance()->GetPrevTimeScale();
				GameManager::Instance()->SetTimeScale(prevScale);
				SetRay(true);
				SetButtons(true);

				// isstart 논리 꼬이는거 수정.
				if (m_startButton)
				{
						auto startScript = m_startButton->GetComponent<ClickStartButton>();
						if (startScript && !m_isStartBattle)		// 게임 시작이 아닌 경우만
						{
								startScript->SetIsStart(false);
						}
				}
		}
}

void RayCastHitEvent::SetButtons(bool value)
{
		// skillbutton, startbutton 둘 다 set 해주기.
		auto skillbutton = m_skillButton->GetComponent<SkillButton>();
		if (skillbutton && m_startButton)
		{
				skillbutton->GetAttackButton()->SetActive(value);
				skillbutton->GetHealButton()->SetActive(value);

				m_startButton->GetComponent<UIButton>()->SetActive(value);
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

EnemyUnit* RayCastHitEvent::CheckEnemyObj(GameObject* obj)
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
			EnemyUnit* unit = m_Unit->GetComponent<EnemyUnit>();
			if (unit)
			{
				return unit;
			}
		}
	}

	return nullptr;
}
