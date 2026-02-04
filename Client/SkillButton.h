#pragma once
#include "MonoBehaviour.h"

class GameObject;
class UIButton;

class SkillButton : public MonoBehaviour
{
		DTGENERATED_BODY(SkillButton);
public:

		void Start() override;

		void UnlockHealSkill() { m_isHealUnlocked = true; }
		void UnlockAttackSkill() { m_isAttackUnlocked = true; }

		bool GetUnlockHealSkill() { return m_isHealUnlocked; }
		bool GetUnlockAttackSkill() { return m_isAttackUnlocked; }

private:
		GameObject* m_rayObj = nullptr;
		UIButton* m_attackButton = nullptr;
		UIButton* m_healButton = nullptr;
		bool m_isAttackUnlocked = false;
		bool m_isHealUnlocked = false;
};