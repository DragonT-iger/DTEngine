#pragma once
#include "UISlider.h"

enum class e_sliderType
{
	bgm,
	sfx,
	max
};


class BGMUISlider : public UISlider
{
	DTGENERATED_BODY(BGMUISlider);

public:
	BGMUISlider() = default;
	~BGMUISlider() = default;

public:
	void Awake() override;
	void Start() override; //-> handle 위치 읽어서 Value 조절하는 거 말고는 없음. 

	void SetNormalizedValue(float normalizedValue);
	void SetValue(float value) override;

private:
	int m_Type = -1;
};