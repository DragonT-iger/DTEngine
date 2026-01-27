#include "pch.h"
#include "UIManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "DX11Renderer.h"
#include "prefab.h"
#include "Camera.h"

#include "PrefabSelectWindow.h"
//#include "RuleSelectWindow.h"
#include "SettingsWindow.h"

BEGINPROPERTY(UIManager)
ENDPROPERTY()

void UIManager::Awake()
{
		InitializeWindows();
}

void UIManager::Update(float deltaTime)
{
		RayUpdate(); // 클릭 감지 및 UI 분기 처리
}

void UIManager::InitializeWindows()
{
		Scene* scene = SceneManager::Instance().GetActiveScene();

		// Prefab Select Window 생성
		GameObject* pswObj = scene->CreateGameObject("UI_PrefabSelectWindow");
		pswObj->GetTransform()->SetParent(this->GetTransform()); // UIManager의 자식으로 등록
		m_prefabSelectWindow = pswObj->AddComponent<PrefabSelectWindow>();
		//m_prefabSelectWindow->CreateOnce(); // 이미 컴포넌트화 되어 있으므로 호출

		// Rule Select Window 생성
		/*GameObject* rswObj = scene->CreateGameObject("UI_RuleSelectWindow");
		rswObj->GetTransform()->SetParent(this->GetTransform());
		m_ruleSelectWindow = rswObj->AddComponent<RuleSelectWindow>();*/

		// Settings Window 생성
		GameObject* swObj = scene->CreateGameObject("UI_SettingsWindow");
		swObj->GetTransform()->SetParent(this->GetTransform());
		m_settingsWindow = swObj->AddComponent<SettingsWindow>();
		//m_settingsWindow->CreateOnce();
}

void UIManager::RayUpdate()
{
		if (m_prefabSelectWindow->IsWindowOpen())
		{
				return;
		}
		auto& input = InputManager::Instance();
		Scene* scene = SceneManager::Instance().GetActiveScene();
		Camera* camera = scene->GetMainCamera();

		if (input.GetKeyDown(KeyCode::MouseLeft) && camera)
		{
				auto mp = input.GetGameMousePosition();

				float viewW = (float)DX11Renderer::Instance().GetWidth();
				float viewH = (float)DX11Renderer::Instance().GetHeight();

				// 창 밖 클릭 방어
				if (mp.x >= 0 && mp.y >= 0 && mp.x < viewW && mp.y < viewH)
				{
						Ray ray = camera->ScreenPointToRay(mp.x, mp.y, viewW, viewH);

						GameObject* hit = nullptr;
						float t = 0.0f;
						if (scene->Raycast2(ray, hit, t))
						{
								// 여기에 뭔가 더 넣으면 될듯.. | 여기에다가 이름 비교해서 cube 인 경우 생성 이정도로 임시 잡아두기.
								//std::cout << hit->GetName() << std::endl;
								HandleEditorHit(hit);
						}
				}
		}
}

void UIManager::HandleEditorHit(GameObject* hit)
{
		if (!hit) return;
		HandlePrefabSelection(hit);
}

void UIManager::HandlePrefabSelection(GameObject* hit)
{
		// cube 아니면 처리 안할거임.
		if (!hit || hit->GetName() != "Cube") return;

		// ui 전부 닫기.
		CloseAllUI();

		// spawnpos 저장해두기.
		Vector3 spawnPos = hit->GetTransform()->GetWorldPosition();

		m_hitObject = hit;

		if (m_prefabSelectWindow)
		{
				m_prefabSelectWindow->Open([this, spawnPos](Prefab* selected)
						{
								if (selected)
								{
										// prefab 정보로 생성.
										GameObject* model = selected->Instantiate();

										// 생성 성공. 설정해주기. 
										if (model)
										{
												model->GetTransform()->SetPosition(spawnPos + Vector3(0.f, 1.f, 0.f));   // tile보다 살짝 위로.
												model->GetTransform()->SetRotationEuler(Vector3(0.f, 90.f, 0.f));        // 바라보는 방향으로 회전.

												if (m_hitObject)
												{
														m_hitObject->SetName("ICube");  // tile 이름 수정으로 이후 생성 못하는걸로.
												}
										}
								}
						}
				);
		}

		//else if (objName == "Knight" || objName == "Bishop" || objName == "Rook")
		//{
		//		if (m_ruleSelectWindow)
		//		{
		//				CloseAllUI();

		//				// 해당 모델과 콜백 전달
		//				m_ruleSelectWindow->Open(hit, [hit](RuleType rule)
		//						{
		//								if (rule != RuleType::None)
		//								{
		//										// 한슬이가 여기서 구현하면 unitcomponent 호출해서 getcomponent에 rule 수정하기.
		//										// hit->GetComponent<UnitScript>()->SetRule(rule);
		//										std::cout << hit->GetName() << "의 규칙이 변경됨" << std::endl;
		//								}
		//						});
		//		}
		//}
}

void UIManager::CloseAllUI()
{
		if (m_prefabSelectWindow) m_prefabSelectWindow->Close();
		//if (m_ruleSelectWindow)   m_ruleSelectWindow->Close();
		if (m_settingsWindow)     m_settingsWindow->Close();
}