#pragma once
#include "MonoBehaviour.h"

class UIButton;
class GameObject;

class ESCEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ESCEvent);
public:
		//void Start() override;
		void Update(float deltaTime) override;

		void Check();
private:
		UIButton* m_settingButton = nullptr;

		GameObject* m_window = nullptr;
		GameObject* m_settingWindow = nullptr;
};

