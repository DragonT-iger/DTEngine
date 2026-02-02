#pragma once
#include "MonoBehaviour.h"

class UIButton;
class GameObject;

class ClickStartButton : public MonoBehaviour
{
		DTGENERATED_BODY(ClickStartButton);
public:

		void Start() override;

private:
		UIButton* m_startButton = nullptr;
		GameObject* m_combatObj = nullptr;
		GameObject* m_rayObj = nullptr;
};

