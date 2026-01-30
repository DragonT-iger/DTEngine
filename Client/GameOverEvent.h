#pragma once
#include "MonoBehaviour.h"

class UIButton;

class GameOverEvent : public MonoBehaviour
{
		DTGENERATED_BODY(GameOverEvent);
public:
		void Start() override;
		
private:
		UIButton* m_button;
};

