#include "ImageVisualUpdateEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "SelectIndexEvent.h"
#include "Image.h"

BEGINPROPERTY(ImageVisualUpdateEvent)
DTPROPERTY(ImageVisualUpdateEvent, m_fButton)
DTPROPERTY(ImageVisualUpdateEvent, m_sButton)
DTPROPERTY(ImageVisualUpdateEvent, m_tButton)
DTPROPERTY(ImageVisualUpdateEvent, m_fBigImgObj)
DTPROPERTY(ImageVisualUpdateEvent, m_sBigImgObj)
DTPROPERTY(ImageVisualUpdateEvent, m_tBigImgObj)
ENDPROPERTY()


void ImageVisualUpdateEvent::Start()
{
		auto selectEvent = _GetOwner()->GetTransform()->GetParent()->_GetOwner()->GetComponent<SelectIndexEvent>();
		if (!selectEvent) return;

		if (m_fButton)
		{
				m_fButton->SetOnClick([this, selectEvent]()
						{
								if (selectEvent)
								{
										selectEvent->SetIndex(1); // 인덱스 변경
										UpdateAllButtonsVisual();
								}
						});
		}

		if (m_sButton) 
		{
				m_sButton->SetOnClick([this,selectEvent]()
						{
								selectEvent->SetIndex(2);
								UpdateAllButtonsVisual();
						});
		}

		if (m_tButton) 
		{
				m_tButton->SetOnClick([this, selectEvent]()
						{
								selectEvent->SetIndex(3);
								UpdateAllButtonsVisual();
						});
		}
}

void ImageVisualUpdateEvent::UpdateAllButtonsVisual()
{
		if (!m_fBigImgObj || !m_sBigImgObj || !m_tBigImgObj) return;

		UIButton* buttons[] = { m_fButton, m_sButton, m_tButton };
		GameObject* bigImages[] = { m_fBigImgObj, m_sBigImgObj, m_tBigImgObj };
		Vector4 selectedColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		Vector4 unselectedColor = Vector4(0.4f, 0.4f, 0.4f, 1.0f);

		auto parent = _GetOwner()->GetTransform()->GetParent()->_GetOwner();
		auto selectEvent = parent->GetComponent<SelectIndexEvent>();
		int index = selectEvent->GetIndex();

		for (int i = 0; i < 3; ++i)
		{
				if (!buttons[i] || !bigImages[i]) continue;

				// i+1 이 현재 선택된 인덱스인지 확인
				bool isSelected = (index == (i + 1));

				// 해당 버튼의 컬러를 명확하게 지정
				buttons[i]->SetNormalColor(isSelected ? selectedColor : unselectedColor);
				buttons[i]->ApplyNormalState();

				if (isSelected)
				{
						// cost, name 끄기.
						for (auto child : buttons[i]->GetTransform()->GetChildren())
						{
								child->_GetOwner()->SetActive(false);
						}

						// 본인 이미지 끄고, 큰 이미지 켜기
						if (auto* img = buttons[i]->GetComponent<Image>()) img->SetActive(false);
						bigImages[i]->SetActive(true);
				}
				else
				{
						// cost, name 끄기.
						for (auto child : buttons[i]->GetTransform()->GetChildren())
						{
								child->_GetOwner()->SetActive(true);
						}

						// 본인 이미지 끄고, 큰 이미지 켜기
						if (auto* img = buttons[i]->GetComponent<Image>()) img->SetActive(true);
						bigImages[i]->SetActive(false);
				}
		}
}