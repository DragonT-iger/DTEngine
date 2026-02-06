#pragma once
#include "MonoBehaviour.h"

class GameObject;
class UIButton;

class SkillButton : public MonoBehaviour
{
		DTGENERATED_BODY(SkillButton);
public:

		void Start() override;

private:
		GameObject* m_rayObj = nullptr;
		UIButton* m_attackButton = nullptr;
		UIButton* m_healButton = nullptr;
};