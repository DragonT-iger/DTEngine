#pragma once

class UIButton;

#include "MonoBehaviour.h"
class SetActiveEvent : public MonoBehaviour
{

	void Start() override;


	DTGENERATED_BODY(SetActiveEvent);


	UIButton* m_button = nullptr;
};

