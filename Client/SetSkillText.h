#pragma once
#include "MonoBehaviour.h"

class Text;

class SetSkillText : public MonoBehaviour
{
		DTGENERATED_BODY(SetSkillText);
public:
		void Awake() override;

private:
		Text* m_Text = nullptr;
};

