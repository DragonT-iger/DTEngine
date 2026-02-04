#include "HPBarFollowEvent.h"
#include "GameObject.h"
#include "Unit.h"
#include "AllyUnit.h"
#include "EnemyUnit.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"

BEGINPROPERTY(HPBarFollowEvent)
DTPROPERTY(HPBarFollowEvent, m_targetObject)
DTPROPERTY(HPBarFollowEvent, m_fillObject)
DTPROPERTY(HPBarFollowEvent, m_lerpSpeed)
ENDPROPERTY()

void HPBarFollowEvent::Awake()
{
		if (!m_mainCamera)
		{
				HPBarCameraSetUp();
		}
}
void HPBarFollowEvent::Start()
{
		if (!m_mainCamera)
		{
				HPBarCameraSetUp();
		}

		if (!m_targetObject || !m_fillObject)
				return;
		
		m_targetUnit = m_targetObject->GetComponent<Unit>();
		m_fillBaseScale = m_fillObject->GetTransform()->GetScale();
		m_fillBasePosition = m_fillObject->GetTransform()->GetPosition();
		m_originalRotation = _GetOwner()->GetTransform()->GetEditorEuler();		
}


void HPBarFollowEvent::Update(float deltaTime)
{
		UpdateFillScale(deltaTime);
}

void HPBarFollowEvent::HPBarCameraSetUp()
{
		Scene* scene = SceneManager::Instance().GetActiveScene();
		m_mainCamera = scene->GetMainCamera();

		Transform* camTf = m_mainCamera->GetComponent<Transform>();
		Vector3 camEuler = camTf->GetEditorEuler();

		// 카메라 반대 방향으로 설정 (Y축 180도 반전)
		Vector3 billboardRot = Vector3(camEuler.x, camEuler.y + 180.0f, camEuler.z);
		_GetOwner()->GetTransform()->SetRotationEuler(billboardRot);

		m_originalRotation = billboardRot;
}

void HPBarFollowEvent::UpdateFillScale(float deltaTime)
{
		if (!m_targetObject || !m_fillObject)
				return;

		// 체력이 아니라 isalive로 판단.
		if (!m_targetUnit->IsAlive())
		{
				_GetOwner()->SetActive(false);
				return;
		}
		
		Transform* parentTf = m_targetObject->GetTransform();
		if (!parentTf)
				return;
		Vector3 parentRot = parentTf->GetEditorEuler();
		Vector3 inverseRot = Vector3(-parentRot.x, -parentRot.y, -parentRot.z);

		// 비율 계산하고
		float targetRatio = m_targetUnit->GetHp() / m_targetUnit->GetStats().maxHp;

		// 보간해서 줄어드는 연출 자연스럽게
		m_currentRatio = Lerp(m_currentRatio, targetRatio, m_lerpSpeed * deltaTime);

		Vector3 fillScale = m_fillObject->GetTransform()->GetScale();

		// Scale 조절. 
		Vector3 newScale = m_fillBaseScale;
		newScale.x = m_fillBaseScale.x * m_currentRatio;
		m_fillObject->GetTransform()->SetScale(newScale);

		// Position 조정
		Vector3 newPos = m_fillBasePosition;
		float offsetX = m_fillBaseScale.x * (1.0f - m_currentRatio);
		newPos.x = m_fillBasePosition.x - offsetX;
		m_fillObject->GetTransform()->SetPosition(newPos);

		// 빌보딩
		Transform* camTf = m_mainCamera->GetComponent<Transform>();
		Vector3 camEuler = camTf->GetEditorEuler();

		// 부모 회전 역행렬에다가 원래 회전값으로.  + 카메라 반대 방향
		Vector3 billboardRot = Vector3(0, camEuler.y + 360.0f, 0);
		Vector3 inverseParentRot = Vector3(0, -parentRot.y, 0);
		Vector3 finalRot = billboardRot + inverseParentRot;
		_GetOwner()->GetTransform()->SetRotationEuler(finalRot);
}

float HPBarFollowEvent::Lerp(float start, float end, float t)
{
		return start + (end - start) * t;
}
