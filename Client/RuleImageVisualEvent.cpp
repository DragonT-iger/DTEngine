#include "RuleImageVisualEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "OpenRSEvent.h"

BEGINPROPERTY(RuleImageVisualEvent)
DTPROPERTY(RuleImageVisualEvent, m_movefBtns)
DTPROPERTY(RuleImageVisualEvent, m_movesBtns)
DTPROPERTY(RuleImageVisualEvent, m_movetBtns)
DTPROPERTY(RuleImageVisualEvent, m_battlefBtns)
DTPROPERTY(RuleImageVisualEvent, m_battlesBtns)
DTPROPERTY(RuleImageVisualEvent, m_battletBtns)
ENDPROPERTY()

void RuleImageVisualEvent::Start()
{
		auto bgEvent = _GetOwner()->GetComponent<OpenRSEvent>();

		// 반복문 쓸려고 배열로.
		UIButton* moveButtons[] = { m_movefBtns, m_movesBtns, m_movetBtns };
		UIButton* battleButtons[] = { m_battlefBtns, m_battlesBtns, m_battletBtns };

		// 이벤트 각각 넣어주기.
		for (int i = 0; i < 3; ++i)
		{
				if (moveButtons[i])
				{
						moveButtons[i]->SetOnClick([this, bgEvent, i]()
								{
										bgEvent->SetMoveIndex(i);
										UpdateMoveVisuals();
								});
				}
				if (battleButtons[i])
				{
						battleButtons[i]->SetOnClick([this, bgEvent, i]()
								{
										bgEvent->SetBattleIndex(i);
										UpdateBattleVisuals();
								});
				}
		}
}

void RuleImageVisualEvent::UpdateAllVisuals()
{
		UpdateMoveVisuals();
		UpdateBattleVisuals();
}

void RuleImageVisualEvent::UpdateMoveVisuals()
{
		auto bgEvent = _GetOwner()->GetComponent<OpenRSEvent>();
		if (!bgEvent) return;

		int mIdx = bgEvent->GetMoveIndex();
		UIButton* moveButtons[] = { m_movefBtns, m_movesBtns, m_movetBtns };
		Vector4 selectedColor(1.0f, 1.0f, 1.0f, 1.0f);
		Vector4 unselectedColor(1.0f, 1.0f, 1.0f, 0.0f);

		for (int i = 0; i < 3; ++i) 
		{
				if (moveButtons[i]) 
				{
						moveButtons[i]->SetNormalColor((mIdx == i) ? selectedColor : unselectedColor);
						moveButtons[i]->ApplyNormalState();
				}
		}
}

void RuleImageVisualEvent::UpdateBattleVisuals()
{
		auto bgEvent = _GetOwner()->GetComponent<OpenRSEvent>();
		if (!bgEvent) return;

		int aIdx = bgEvent->GetBattleIndex();
		UIButton* battleButtons[] = { m_battlefBtns, m_battlesBtns, m_battletBtns };
		Vector4 selectedColor(1.0f, 1.0f, 1.0f, 1.0f);
		Vector4 unselectedColor(1.0f, 1.0f, 1.0f, 0.0f);

		for (int i = 0; i < 3; ++i) 
		{
				if (battleButtons[i]) 
				{
						battleButtons[i]->SetNormalColor((aIdx == i) ? selectedColor : unselectedColor);
						battleButtons[i]->ApplyNormalState();
				}
		}
}
