#include "RayCastHitEvent.h"
#include "GameObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "DX11Renderer.h"
#include "Camera.h"
#include "OpenPSEvent.h"
#include "OpenRSEvent.h"

BEGINPROPERTY(RayCastHitEvent)
DTPROPERTY(RayCastHitEvent, m_PSWinodwBG)
DTPROPERTY(RayCastHitEvent, m_RSWinodwBG)
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
				auto mp = input.GetGameMousePosition();

				// UI 창이 활성화되어 있다면 해당 영역 체크
				if (m_PSWinodwBG && m_PSWinodwBG->IsActive())
				{
						Transform* uiTf = m_PSWinodwBG->GetTransform();
						Vector3 pos = uiTf->GetPosition();
						Vector3 scale = uiTf->GetScale();

						// 중앙 기준 Rect 범위 계산
						float minX = pos.x - (scale.x * 0.5f);
						float maxX = pos.x + (scale.x * 0.5f);
						float minY = pos.y - (scale.y * 0.5f);
						float maxY = pos.y + (scale.y * 0.5f);

						// 마우스가 UI 영역 안에 있는지 검사
						if (mp.x >= minX && mp.x <= maxX &&
								mp.y >= minY && mp.y <= maxY)
						{
								// UI 영역을 클릭했으므로 월드 레이캐스트(큐브 선택) 무시
								return;
						}
				}

				float x = mp.x;
				float y = mp.y;
				Ray r = camera->ScreenPointToRay(x, y);
				GameObject* hit = nullptr;
				float hitT = 0.0f;

				scene->Raycast2(r, hit, hitT);

				if (hit)
				{
						std::cout << hit->GetName() << std::endl;

						if (m_PSWinodwBG && hit->GetName() == "Cube")
						{
								m_PSWinodwBG->GetComponent<OpenPSEvent>()->SetRayObj(hit);
								m_PSWinodwBG->GetComponent<OpenPSEvent>()->SetActivePSWindow();
						}
						else
						{
								std::cout << "WindowBG null임" << std::endl;
						}

						if (m_RSWinodwBG && hit->GetName() == "Knight" || hit->GetName() == "Bishop" || hit->GetName() == "Rook")
						{
								m_PSWinodwBG->GetComponent<OpenRSEvent>()->SetTargetUnit(hit);
						}
				}

				
		}
}