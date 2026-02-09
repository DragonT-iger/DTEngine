#pragma once
#include "MonoBehaviour.h"

class UIButton;
class GameObject;

class ExitButtonEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ExitButtonEvent);
public:
		void Start() override;
private:
		UIButton* m_exitButton;
		GameObject* m_settingWindow;
};

