#pragma once
#include "MonoBehaviour.h"

class UIButton;
class TitleCreditTriggerEvent : public MonoBehaviour
{

	DTGENERATED_BODY(TitleCreditTriggerEvent);


public:
	TitleCreditTriggerEvent() = default;
	virtual ~TitleCreditTriggerEvent() = default;

public:
	void Start() override;
private:
	UIButton* m_Button = nullptr;
	UIButton* m_Credit = nullptr;

	UIButton* m_Setting = nullptr;
};