#pragma once
#include "MonoBehaviour.h"

class UIButton;

class ExitButtonEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ExitButtonEvent);
public:
		void Start() override;
private:
		UIButton* m_exitButton;
};

