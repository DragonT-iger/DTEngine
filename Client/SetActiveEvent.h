#pragma once

class UIButton;

#include "MonoBehaviour.h"
class SetActiveEvent : public MonoBehaviour
{
		DTGENERATED_BODY(SetActiveEvent);
public:
	void Start() override;

private:

	UIButton* m_button = nullptr;
};

