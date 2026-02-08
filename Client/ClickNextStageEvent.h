#pragma once
#include "MonoBehaviour.h"

class UIButton;
class GameObject;

class ClickNextStageEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ClickNextStageEvent);
public:
		void Start() override;

private:
		UIButton* m_button = nullptr;
		GameObject* m_window = nullptr;
};

