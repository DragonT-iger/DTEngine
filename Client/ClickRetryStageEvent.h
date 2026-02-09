#pragma once
#include "MonoBehaviour.h"

class UIButton;
class GameObject;

class ClickRetryStageEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ClickRetryStageEvent);
public:
		void Start() override;

private:
		UIButton* m_button = nullptr;
		GameObject* m_window = nullptr;
};

