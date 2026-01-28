#pragma once
#include "MonoBehaviour.h"

class Scene;

class RayCastHitEvent : public MonoBehaviour
{
		DTGENERATED_BODY(RayCastHitEvent);
public:
		void Update(float deltaTime) override;	  // ray check 해주기.

		void RaycastCheck();

		GameObject* GetHitObject() { return m_hitObj; }
		void SetHitObject(GameObject* hitobj) { m_hitObj = hitobj; }

private:
		GameObject* m_hitObj = nullptr;

		GameObject* m_PSWinodwBG = nullptr;
		GameObject* m_RSWinodwBG = nullptr;
};

