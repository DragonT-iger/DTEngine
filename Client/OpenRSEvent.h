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
		// image settexure 어캐하지 음.. 
		GameObject* m_imageFirst = nullptr;
		GameObject* m_imageSecond = nullptr;
		GameObject* m_imageThird = nullptr;

		// 생성 유닛 및 image 변경을 위해서
		GameObject* m_targetUnit = nullptr;
		GameObject* m_windowBar = nullptr;

		// 인덱스 있는 경우 image color 수정해줘야함.
		int m_moveIndex = 0;
		int m_battleIndex = 0;
};

