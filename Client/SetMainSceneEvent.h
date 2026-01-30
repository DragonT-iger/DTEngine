#pragma once
#include "MonoBehaviour.h"

class UIButton;

class SetMainSceneEvent : public MonoBehaviour	
{
		DTGENERATED_BODY(SetMainSceneEvent);
public:
		void Start() override;
private:
		UIButton* m_button;
};

