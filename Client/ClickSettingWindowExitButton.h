#pragma once
#include "MonoBehaviour.h"

class GameObject;

class ClickSettingWindowExitButton : public MonoBehaviour
{
		DTGENERATED_BODY(ClickSettingWindowExitButton);
public:
		void Start() override;
		
private:
		GameObject* m_rayObj = nullptr;
};

