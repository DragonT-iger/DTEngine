#include "SelectOkayEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "TilemapGenerator.h"
#include "Prefab.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"
#include "InputManager.h"
#include "Image.h"
#include "AllyUnit.h"
#include "Unit.h"
#include "RuleSelectWindow.h"

BEGINPROPERTY(SelectOkayEvent)
DTPROPERTY(SelectOkayEvent, m_mapData)
DTPROPERTY(SelectOkayEvent, m_ruleWindow)
//DTPROPERTY(SelectOkayEvent, m_ruleWindowBarImg)
//DTPROPERTY(SelectOkayEvent, m_rookTexture)
//DTPROPERTY(SelectOkayEvent, m_knightTexture)
//DTPROPERTY(SelectOkayEvent, m_bishopTexture)
ENDPROPERTY()

static UnitType ToPieceType(const std::string& s)
{
		if (s == "Rook") return UnitType::Rook;
		if (s == "Knight") return UnitType::Knight;
		if (s == "Bishop") return UnitType::Bishop;
		return UnitType::Rook;
}

void SelectOkayEvent::Start()
{
		m_button = GetComponent<UIButton>();
		if (!m_button)
				return;

		m_button->SetOnClick([this]()
				{
						//printf("[DEBUG] Clicked OkayButton. Object Address: %p, SelectIndex: %d\n", this, this->m_selectIndex);
						// 결국 값이 선택했으니 확인 누르면 창 toggle.

						Prefab* value = nullptr;
						switch (m_selectIndex)
						{
						case 1: value = m_mapData->m_prefab0; break;
						case 2: value = m_mapData->m_prefab1; break;
						case 3: value = m_mapData->m_prefab2; break;
						default:value = nullptr;       break;
						}

						if (!value)
						{
								// 실패인거니까 그냥 끄기. 및 실패 반환.
								_GetOwner()->GetTransform()->GetParent()->_GetOwner()->SetActive(false);
								// call back nullptr 보내주면 끝나는거니까.
								//m_onResult(nullptr);
								std::cout << value << std::endl;

								return;
						}

				//		Scene* curScene = SceneManager::Instance().GetActiveScene();
				//		Camera* mainCamera = curScene->GetMainCamera();

				//		float x = InputManager::Instance().GetGameMousePosition().x;
				//		float y = InputManager::Instance().GetGameMousePosition().y;
				//		//Ray r = mainCamera->ScreenPointToRay(x, y);
				//		GameObject* hit;
				//		float hitT = 0.0f;
				//		//curScene->Raycast2(r, hit, hitT);
				//		std::string modelName;

				//		bool createModel = false;
				//		if (!hit && !value)
				//		{
				//				// pos 저장해두기.
				//				Vector3 spawnPos = hit->GetTransform()->GetWorldPosition();

				//				// prefab 정보로 생성.
				//				GameObject* model = value->Instantiate();

				//				// 여기서 만약 if(m_maxUnitCount <= GameManager::Instance()->GetUnitCount) 이거 안에서 if(model 넣기.
				//				// 생성 성공. 설정해주기.
				//				if (model)
				//				{
				//						modelName = model->GetName();
				//						model->GetTransform()->SetPosition(spawnPos + Vector3(0.f, 1.f, 0.f));   // tile보다 살짝 위로.
				//						model->GetTransform()->SetRotationEuler(Vector3(0.f, 90.f, 0.f));        // 바라보는 방향으로 회전.
				//						hit->SetName("ICube");  // tile 이름 수정으로 이후 생성 못하는걸로.
				//						createModel = true;
				//				}
				//		}

				//		// 생성 실패 시 이후 selectwindow open 안함.
				//		if (!createModel) return;

				//		// model 이름 enum으로 변환.
				//		UnitType m_modelType = ToPieceType(modelName);

				//		// select window 끄기.
				//		_GetOwner()->SetActive(false);

				//		// 걍 open만 해주자.
				//		if (!m_ruleWindow)
				//		{
				//				// rule select window 켜주기.
				//				//m_ruleWindow->GetComponent<RuleSelectWindow>()->Open()
				//		}
				//		else
				//		{
				//				std::cout << "rulewindow 안넣었다." << std::endl;
				//		}
				//});
}

void SelectOkayEvent::SetSelectIndex(int val)
{
		if (val <= 0 || val > 3)
				return;

		m_selectIndex = val;
}