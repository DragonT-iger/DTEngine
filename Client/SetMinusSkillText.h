#pragma once
#include "MonoBehaviour.h"

class Text;

class SetMinusSkillText : public MonoBehaviour
{
		DTGENERATED_BODY(SetMinusSkillText);
public:
		void Awake() override;

private:
		Text* m_text = nullptr;
};

