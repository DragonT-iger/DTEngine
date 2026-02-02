#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class Scene;

class RayCastHitEvent : public MonoBehaviour
{
		DTGENERATED_BODY(RayCastHitEvent);
public:
		void Update(float deltaTime) override;	  // ray check 해주기.

		void RaycastCheck();

		GameObject* GetHitObject() { return m_hitObj; }
		void SetHitObject(GameObject* hitobj) { m_hitObj = hitobj; }
		bool IsMouseInUI(GameObject* uiObj, const Vector2& mousePos);
		void CloseAllWindows();

		void SetHealSkill(bool healSkill) { m_isHealSkillOn = healSkill; }
		void SetAttackSkill(bool attackSkill) { m_isAttackSkillOn = attackSkill; }

		bool GetHealSkill() { return m_isHealSkillOn; }
		bool GetAttackSkill() { return m_isAttackSkillOn; }

private:
		GameObject* m_hitObj = nullptr;

		GameObject* m_rightPSWinodwBG = nullptr;
		GameObject* m_rightRSWinodwBG = nullptr;

		GameObject* m_leftPSWinodwBG = nullptr;
		GameObject* m_leftRSWinodwBG = nullptr;

		bool m_isHealSkillOn = false;
		bool m_isAttackSkillOn = false;
};

