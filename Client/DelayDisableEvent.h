#pragma once
#include "MonoBehaviour.h"
class DelayDisableEvent : public MonoBehaviour
{
		DTGENERATED_BODY(DelayDisableEvent);
public:
		void Update(float deltaTime) override;

		void SetTimer(float time) { timer = time; }
private:
		float timer = 0.0f;
		float delay = 1.0f;
};

