#pragma once
#include "MonoBehaviour.h"

class UIButton;
class GameObject;
class Image;


class ClickStartButton : public MonoBehaviour
{
		DTGENERATED_BODY(ClickStartButton);
public:

		void Start() override;
		void StartGame();
		void SetIsStart(bool value) { isStart = value; }
		
private:
		UIButton* m_startButton = nullptr;
		GameObject* m_combatObj = nullptr;
		GameObject* m_rayObj = nullptr;
		GameObject* m_tileMapObj = nullptr;

		Image* m_startClickImg = nullptr;

		int m_maxCount = 3;
		int m_cost = 10;

		bool isStart = false;
};

