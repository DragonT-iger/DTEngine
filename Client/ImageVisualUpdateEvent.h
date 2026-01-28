#pragma once
#include "MonoBehaviour.h"


class UIButton;

class ImageVisualUpdateEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ImageVisualUpdateEvent);
public:
		void Start() override;
		void Update(float deltaTime) override;
		void UpdateAllButtonsVisual();
		void SetVisualIndex(int value) { m_visualIndex = value; }
		const int& GetVisualIndex() const { return m_visualIndex; }
private:
		UIButton* m_fButton = nullptr;
		UIButton* m_sButton = nullptr;
		UIButton* m_tButton = nullptr;
		int m_visualIndex = 0;
};

