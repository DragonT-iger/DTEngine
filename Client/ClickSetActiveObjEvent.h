#pragma once
#include "MonoBehaviour.h"

class GameObject;
class UIButton;
class ClickSetActiveObjEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ClickSetActiveObjEvent);
public:
		void Start() override;

private:
		GameObject* m_obj = nullptr;
		GameObject* m_windowObj = nullptr;
		UIButton* m_button = nullptr;
};

