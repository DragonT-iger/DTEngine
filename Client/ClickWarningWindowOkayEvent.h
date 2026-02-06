#pragma once
#include "MonoBehaviour.h"

class GameObject;


class ClickWarningWindowOkayEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ClickWarningWindowOkayEvent);
public:
		void Start() override;
private:
		GameObject* m_window = nullptr;
		GameObject* m_startButton = nullptr;
};

