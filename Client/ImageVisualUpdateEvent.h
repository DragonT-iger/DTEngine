#pragma once
#include "MonoBehaviour.h"


class UIButton;
class GameObject;

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

		// 이미지 큰거 관리하기 위해서 한개 더 추가.
		GameObject* m_fBigImgObj = nullptr;
		GameObject* m_sBigImgObj = nullptr;
		GameObject* m_tBigImgObj = nullptr;
		int m_visualIndex = 0;
};

