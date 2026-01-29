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
				const Vector2 mp = Vector2(input.GetGameMousePosition().x, input.GetGameMousePosition().y);

				// 둘 중 한개라도 활성 상태고 마우스가 그 위에 있는지 체크.
				if (IsMouseInUI(m_PSWinodwBG, mp) || IsMouseInUI(m_RSWinodwBG, mp))
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

				if (hit)
				{
						std::cout << hit->GetName() << std::endl;
						if (hit->GetName() == "Height_01_White_Test" || hit->GetName() == "Height_01_Red_Test")
						{
								if (m_PSWinodwBG)
								{
										CloseAllWindows(); // 먼저 다 닫고
										auto psEvent = m_PSWinodwBG->GetComponent<OpenPSEvent>();
										psEvent->SetRayObj(hit);
										psEvent->SetActivePSWindow(); // PS 오픈
								}
						}

						// RS 관련 오브젝트 클릭 시
						else if (hit->GetName() == "Chess" || hit->GetName() == "Wand" || hit->GetName() == "Shield" 
								|| hit->GetName() == "Sword" || hit->GetName() == "Spear" || hit->GetName() == "Shield")
						{ 
								if (m_RSWinodwBG)
								{
										CloseAllWindows(); // 먼저 다 닫고
										m_RSWinodwBG->GetComponent<OpenRSEvent>()->RequestOpenWindow(hit); // RS 오픈
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

		float minX = pos.x - (scale.x * 0.5f);
		float maxX = pos.x + (scale.x * 0.5f);
		float minY = pos.y - (scale.y * 0.5f);
		float maxY = pos.y + (scale.y * 0.5f);

		return (mousePos.x >= minX && mousePos.x <= maxX &&
				mousePos.y >= minY && mousePos.y <= maxY);
}

void RayCastHitEvent::CloseAllWindows()
{
		if (m_PSWinodwBG) m_PSWinodwBG->SetActive(false);
		if (m_RSWinodwBG) m_RSWinodwBG->SetActive(false);
}
