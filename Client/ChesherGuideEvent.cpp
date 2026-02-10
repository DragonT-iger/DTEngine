#include "ChesherGuideEvent.h"
#include "GameObject.h"
#include "UIButton.h"
#include "InputManager.h"

#include "RayCastHitEvent.h"

BEGINPROPERTY(ChesherGuideEvent)
DTPROPERTY(ChesherGuideEvent, m_rayObj)
DTPROPERTY(ChesherGuideEvent, m_battleSkillButton)
DTPROPERTY(ChesherGuideEvent, m_healSkillButton)
DTPROPERTY(ChesherGuideEvent, m_settingButton)
DTPROPERTY(ChesherGuideEvent, m_startButton)
ENDPROPERTY()

void ChesherGuideEvent::Start()
{
		if (!m_battleSkillButton || !m_healSkillButton || !m_settingButton || !m_startButton || !m_rayObj)
				return;

		// 상호작용 안먹게 다 꺼주기.
		SetActiveUI(false);

		// 본인 버튼이야 이게 실질적인 버튼 이미지
		m_button = GetComponent<UIButton>();

		// 내 버튼 클릭되면 실행될 콜백 함수.
		m_button->SetOnClick([this]() 
				{
						auto ray = m_rayObj->GetComponent<RayCastHitEvent>();
						bool isray = ray->GetRay();
						if (isray)
						{
								ray->SetRay(true);
						}
						// 다시 상호작용 가능하게 켜주기.
					CloseGuide();
				});
}


void ChesherGuideEvent::Update(float deltaTime)
{
	if (InputManager::Instance().GetKeyDown(KeyCode::Enter) ||
		InputManager::Instance().GetKeyDown(KeyCode::Space))
	{
		CloseGuide();
	}
}

void ChesherGuideEvent::CloseGuide()
{
	SetActiveUI(true);

	_GetOwner()->GetTransform()->GetParent()->_GetOwner()->SetActive(false);
}

void ChesherGuideEvent::SetActiveUI(bool b)
{
		m_rayObj->SetActive(b);
		m_battleSkillButton->SetActive(b);
		m_healSkillButton->SetActive(b);
		m_settingButton->SetActive(b);
		m_startButton->SetActive(b);
}
