#include "SetSkillText.h"
#include "GameObject.h"
#include "Text.h"


BEGINPROPERTY(SetSkillText)
DTPROPERTY(SetSkillText, m_Text)
ENDPROPERTY()


void SetSkillText::Awake()
{
		if (!m_Text)
				return;

		m_Text->SetText(L"+1");
}
