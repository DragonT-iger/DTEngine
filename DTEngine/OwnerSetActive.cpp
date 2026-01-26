#include "pch.h"
#include "OwnerSetActive.h"
#include "UIButton.h"


BEGINPROPERTY(OwnerSetActive)
DTPROPERTY_ACCESSOR(OwnerSetActive, m_target, GetTarget, SetTarget)
ENDPROPERTY()

void OwnerSetActive::Awake()
{
		// button 없으면 안되게 만들거임. 
		UIButton* button = _GetOwner()->GetComponent<UIButton>();
		if (!button)
				return;

		button->SetOnClick([this]() { ToggleTarget(); });
}

void OwnerSetActive::ToggleTarget()
{
		// target toggle해주기.
		if (!m_target)
				return;
		m_target->SetActive(!m_target->IsActive());
}
