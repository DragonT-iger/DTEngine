#pragma once
#include "MonoBehaviour.h"

class GameObject;

class ESCEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ESCEvent);
public:
		//void Start() override;
		void Update(float deltaTime) override;
		void SetCreditOpen(bool isOpen) { m_isCreditOpen = isOpen; }

		void Check();
private:

		bool m_isCreditOpen = false;
		
		GameObject* m_window = nullptr;
		GameObject* m_settingWindow = nullptr;
};

