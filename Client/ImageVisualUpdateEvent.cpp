#include "ImageVisualUpdateEvent.h"
#include "UIButton.h"
#include "GameObject.h"
#include "SelectIndexEvent.h"

BEGINPROPERTY(ImageVisualUpdateEvent)
DTPROPERTY_ACCESSOR(ImageVisualUpdateEvent, m_visualIndex, GetVisualIndex, SetVisualIndex)
DTPROPERTY(ImageVisualUpdateEvent, m_fButton)
DTPROPERTY(ImageVisualUpdateEvent, m_sButton)
DTPROPERTY(ImageVisualUpdateEvent, m_tButton)
ENDPROPERTY()


void ImageVisualUpdateEvent::Update(float deltaTime)
{
		// 어차피 아래 함수에서 return 처리 다 하는 중.
		//UpdateAllButtonsVisual();
}

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
		UIButton* buttons[] = { m_fButton, m_sButton, m_tButton };
		Vector4 selectedColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		Vector4 unselectedColor = Vector4(0.4f, 0.4f, 0.4f, 1.0f);

		auto parent = _GetOwner()->GetTransform()->GetParent()->_GetOwner();
		auto selectEvent = parent->GetComponent<SelectIndexEvent>();
		int index = selectEvent->GetIndex();
		if (!selectEvent)
				std::cout << "event 컴포넌트 없다 index 어캐하냐" << std::endl;

		for (int i = 0; i < 3; ++i)
		{
				if (!buttons[i]) continue;

				// i+1 이 현재 선택된 인덱스인지 확인
				bool isSelected = (index == (i + 1));

				// 해당 버튼의 컬러를 명확하게 지정
				buttons[i]->SetNormalColor(isSelected ? selectedColor : unselectedColor);
				 buttons[i]->ApplyNormalState();
		}
}
