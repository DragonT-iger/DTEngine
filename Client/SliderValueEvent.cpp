#include "SliderValueEvent.h"
#include "UISlider.h"
#include "GameManager.h"
#include "GameObject.h"
#include "AssetDatabase.h"

BEGINPROPERTY(SliderValueEvent)
DTPROPERTY(SliderValueEvent, m_slider)
ENDPROPERTY()

void SliderValueEvent::Start()
{
		if (!m_slider)
				return;

		// 배경음 효과음 slider 각각. 대신 이름 달라지면 못찾음. 따로 event 한개 더 안만들려고 한거라 prefab 이름 수정 x 
		if (_GetOwner()->GetName() == "BGMSliderBar")
		{
				m_slider->SetOnValueChanged([this]()
						{
								GameManager::Instance()->SetBgmValue(m_slider->GetValue());
						});
		}

		if (_GetOwner()->GetName() == "SFXSliderBar")
		{
				m_slider->SetOnValueChanged([this]()
						{
								GameManager::Instance()->SetSfxValue(m_slider->GetValue());
						});
		}
}
