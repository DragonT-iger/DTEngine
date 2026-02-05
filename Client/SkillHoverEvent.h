#pragma once
#include "MonoBehaviour.h"

class GameObject;
class UIButton;

class SkillHoverEvent : public MonoBehaviour
{
		DTGENERATED_BODY(SkillHoverEvent);
public:
		void Start() override;
		void Update(float deltaTime) override;

private:
		GameObject* m_skillWindowObj = nullptr;
		UIButton* m_skillButton = nullptr;
};

