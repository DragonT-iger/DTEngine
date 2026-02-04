#pragma once
#include "MonoBehaviour.h"

class GameObject;

class OpenRSEvent : public MonoBehaviour
{
		DTGENERATED_BODY(OpenRSEvent);

public:
		GameObject* GetTargetUnit() { return m_targetUnit; }

		void OpenWindow();
		void RequestOpenWindow(GameObject* target);		// 함수 한개로 통일.
		void SetMoveIndex(int idx) { m_moveIndex = idx; }
		int GetMoveIndex() { return m_moveIndex; }

		void SetBattleIndex(int idx) { m_battleIndex = idx; }
		int GetBattleIndex() { return m_battleIndex; }

private:
		// 생성 유닛 및 image 변경을 위해서
		GameObject* m_targetUnit = nullptr;
		GameObject* m_windowBar = nullptr;

		// rule info window 꺼지게.
		GameObject* m_moveRuleWindow = nullptr;
		GameObject* m_battleRuleWindow = nullptr;

		// 인덱스 있는 경우 image color 수정해줘야함.
		int m_moveIndex = 0;
		int m_battleIndex = 0;
};

