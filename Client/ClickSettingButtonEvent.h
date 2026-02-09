#pragma once
#include "MonoBehaviour.h"

class UIButton;
class GameObject;

class ClickSettingButtonEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ClickSettingButtonEvent);
public:
		void Start() override;
private:
		GameObject* m_rayobj = nullptr;
		UIButton* m_button = nullptr;
};