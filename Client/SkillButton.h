#pragma once
#include "MonoBehaviour.h"

class GameObject;
class UIButton;

class SkillButton : public MonoBehaviour
{
		DTGENERATED_BODY(SkillButton);
public:

		void Start() override;

		// ray 떄 uibutton 동작처리 안하게 할려고 가져오기.
		UIButton* GetAttackButton() { return m_attackButton; }
		UIButton* GetHealButton() { return m_healButton; }
private:
		GameObject* m_rayObj = nullptr;
		UIButton* m_attackButton = nullptr;
		UIButton* m_healButton = nullptr;
};