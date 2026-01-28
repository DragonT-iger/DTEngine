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
										bgEvent->SetMoveIndex(i + 1);
										UpdateVisuals();
								});
				}
				if (battleButtons[i])
				{
						battleButtons[i]->SetOnClick([this, bgEvent, i]()
								{
										bgEvent->SetBattleIndex(i + 1);
										UpdateVisuals();
								});
				}
		}
}

void RuleImageVisualEvent::UpdateVisuals()
{
		auto bgEvent = _GetOwner()->GetComponent<OpenRSEvent>();
		if (!bgEvent) return;

		int mIdx = bgEvent->GetMoveIndex();
		int aIdx = bgEvent->GetBattleIndex();

		// 반복문 쓸려고 배열로.
		UIButton* moveButtons[] = { m_movefBtns, m_movesBtns, m_movetBtns };
		UIButton* battleButtons[] = { m_battlefBtns, m_battlesBtns, m_battletBtns };

		Vector4 selectedColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		Vector4 unselectedColor = Vector4(0.4f, 0.4f, 0.4f, 1.0f);

		for (int i = 0; i < 3; ++i)
		{
				// 이동 버튼 처리
				if (moveButtons[i])
				{
						bool isSelected = (mIdx == (i + 1));
						moveButtons[i]->SetNormalColor(isSelected ? selectedColor : unselectedColor);
						moveButtons[i]->ApplyNormalState(); // 즉시 반영
				}

				// 공격(배틀) 버튼 처리
				if (battleButtons[i])
				{
						bool isSelected = (aIdx == (i + 1));
						battleButtons[i]->SetNormalColor(isSelected ? selectedColor : unselectedColor);
						battleButtons[i]->ApplyNormalState();
				}
		}
}
