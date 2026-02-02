#pragma once
#include "MonoBehaviour.h"


class UIButton;

class ImageVisualUpdateEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ImageVisualUpdateEvent);
public:
		void Start() override;
		void UpdateAllButtonsVisual();
		
private:
		UIButton* m_fButton = nullptr;
		UIButton* m_sButton = nullptr;
		UIButton* m_tButton = nullptr;
		int m_visualIndex = 0;
};

