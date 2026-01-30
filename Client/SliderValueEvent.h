#pragma once
#include "MonoBehaviour.h"

class UISlider;

class SliderValueEvent : public MonoBehaviour
{
		DTGENERATED_BODY(SliderValueEvent);
public:
		void Start() override;
private:
		UISlider* m_slider = nullptr;
};

