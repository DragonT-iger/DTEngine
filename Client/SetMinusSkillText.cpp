#include "SetMinusSkillText.h"
#include "GameObject.h"
#include "Text.h"


BEGINPROPERTY(SetMinusSkillText)
DTPROPERTY(SetMinusSkillText, m_text)		
ENDPROPERTY()


void SetMinusSkillText::Awake()
{
		if (!m_text)
				return;

		m_text->SetText(L"-1");
}
