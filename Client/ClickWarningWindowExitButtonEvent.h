#pragma once
#include "MonoBehaviour.h"

class GameObject;

class ClickWarningWindowExitButtonEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ClickWarningWindowExitButtonEvent);
public:
		void Start() override;

private:
		GameObject* m_startButton = nullptr;
		GameObject* m_rayObj = nullptr;
};

