#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class GameObject;
class Unit;
class Camera;

class HPBarFollowEvent : public MonoBehaviour
{
		DTGENERATED_BODY(HPBarFollowEvent);
public:
		void Awake() override;
		void Start() override;
		void Update(float deltaTime) override;

		void HPBarCameraSetUp();
		void UpdateFillScale(float deltaTime);
		float Lerp(float start, float end, float t);

		void SetTargetObject(GameObject* obj) { m_targetObject = obj; }
		void SetFillObject(GameObject* obj) { m_fillObject = obj; }
private:
		// target
		GameObject* m_targetObject = nullptr;
		Unit* m_targetUnit = nullptr;

		// fill obj
		GameObject* m_fillObject = nullptr;
		
		// 체력바 연출 설정.
		float m_lerpSpeed = 8.0f;       // 값이 클수록 빠르게 따라감.
		float m_currentRatio = 1.0f;    // 현재 화면에 표시된 비율.

		// fill의 원래 크기.
		Vector3 m_fillBaseScale = Vector3(1.0f, 1.0f, 1.0f);
		Vector3 m_fillBasePosition = Vector3(0.0f, 0.0f, 0.0f);

		// 내 원래 rotation 값.
		Vector3 m_originalRotation = Vector3(1.0f, 1.0f, 1.0f);

		// maincamera에 맞춰서 각도 조절할려고.
		Camera* m_mainCamera = nullptr;
};

