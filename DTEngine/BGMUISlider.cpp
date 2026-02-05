#include "pch.h"
#include "BGMUISlider.h"
#include "../Client/GameManager.h"


BEGINPROPERTY(BGMUISlider)
DTPROPERTY(BGMUISlider , m_Type)
ENDPROPERTY()




void BGMUISlider::Awake()
{
	__super::Awake();
	if (m_Type < 0) return;
}

//GameManager로 부터 값을 받아서 slider 위치를 이동 
void BGMUISlider::Start()
{
	if (m_Type < 0) return;

	else if (m_Type == static_cast<int>(e_sliderType::bgm))
	{
		float value = GameManager::Instance()->GetBgmValue();
		SetNormalizedValue(value);
	}

	else if (m_Type == static_cast<int>(e_sliderType::sfx))
	{
		float value = GameManager::Instance()->GetSfxValue();
		SetNormalizedValue(value);
	}
}

void BGMUISlider::SetNormalizedValue(float normalizedValue)
{
	
	float t = std::clamp(normalizedValue, 0.0f, 1.0f);


	float newValue = m_minValue + (m_maxValue - m_minValue) * t;

	SetValue(newValue);

	

	UpdateHandlePosition();
}

void BGMUISlider::SetValue(float value)
{
	if (m_wholeNumbers)
	{
		value = std::round(value);
	}

	float clamped = std::clamp(value, m_minValue, m_maxValue);

	if (clamped == m_value) return;

	m_value = clamped;


	if (m_Type == static_cast<int>(e_sliderType::bgm))
	{
		GameManager::Instance()->SetBgmValue(m_value);
	}
	else if (m_Type == static_cast<int>(e_sliderType::sfx))
	{
		GameManager::Instance()->SetSfxValue(m_value);
	}

	InvokeValueChanged();
}

