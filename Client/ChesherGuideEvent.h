#pragma once
#include "MonoBehaviour.h"

class GameObject;
class UIButton;
class CameraRotator;

class ChesherGuideEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ChesherGuideEvent);
public:
		void Start() override;
		void SetActiveUI(bool b);
		void Update(float deltaTime) override; 
		
private:
		void CloseGuide();
		GameObject* m_rayObj = nullptr;
		CameraRotator* m_cameraRotator = nullptr;

		UIButton* m_battleSkillButton = nullptr;
		UIButton* m_healSkillButton = nullptr;
		UIButton* m_settingButton = nullptr;
		UIButton* m_startButton = nullptr;

		UIButton* m_button = nullptr;
};

