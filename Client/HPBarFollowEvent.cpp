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
		Vector3 camEuler = camTf->GetWorldRotationEuler();

		// 카메라 반대 방향으로 설정 (Y축 180도 반전)
		Vector3 billboardRot = Vector3(camEuler.x, camEuler.y + 180.0f, camEuler.z);
		_GetOwner()->GetTransform()->SetRotationEuler(billboardRot);

		m_originalRotation = billboardRot;
}

void HPBarFollowEvent::UpdateFillScale(float deltaTime)
{
    if (!m_targetObject || !m_fillObject)
        return;

    if (!m_targetUnit->IsAlive())
    {
        _GetOwner()->SetActive(false);
        return;
    }

    float targetRatio = m_targetUnit->GetHp() / m_targetUnit->GetStats().maxHp;
    m_currentRatio = Lerp(m_currentRatio, targetRatio, m_lerpSpeed * deltaTime);

    Vector3 fillScale = m_fillObject->GetTransform()->GetScale();
    Vector3 newScale = m_fillBaseScale;
    newScale.x = m_fillBaseScale.x * m_currentRatio;
    m_fillObject->GetTransform()->SetScale(newScale);

    Vector3 newPos = m_fillBasePosition;
    float offsetX = m_fillBaseScale.x * (1.0f - m_currentRatio);
    newPos.x = m_fillBasePosition.x - offsetX;
    m_fillObject->GetTransform()->SetPosition(newPos);


    if (m_mainCamera)
    {
        Transform* camTf = m_mainCamera->GetComponent<Transform>();
        Transform* parentTf = m_targetObject->GetTransform();

        Vector3 camWorldEuler = camTf->GetWorldRotationEuler();
        Vector3 parentWorldEuler = parentTf->GetWorldRotationEuler();

        Vector3 targetWorldEuler = camWorldEuler;
        targetWorldEuler.y += 180.0f;

        Vector3 localEuler = targetWorldEuler - parentWorldEuler;
        localEuler.x += 90.0f;


        _GetOwner()->GetTransform()->SetRotationEuler(localEuler);
    }
}

float HPBarFollowEvent::Lerp(float start, float end, float t)
{
		return start + (end - start) * t;
}
