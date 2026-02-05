#pragma once

class UIButton;
class GameObject;

#include "MonoBehaviour.h"
class MakeSetActive : public MonoBehaviour
{
	DTGENERATED_BODY(MakeSetActive);
public:
	void Start() override;

private:
	UIButton* m_toBeActivated_button = nullptr;
	UIButton* m_button = nullptr;

	GameObject* m_objWindow = nullptr;
};

