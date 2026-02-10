#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class GameObject;
class Image;
class Text;
class UIButton;


class BgFadeController : public MonoBehaviour
{
		DTGENERATED_BODY(BgFadeController);
public:
		void Start() override;
		void Update(float deltaTime) override;
		void BGFadeUpdate(float deltaTime);

		void Init();
		void SetVictoryObj(bool value);
		void SetDefeatObj(bool value);

		void SettingToggleFinish();
private:
		GameObject* m_settingButton = nullptr;
		GameObject* m_rayObj = nullptr;
		GameObject* m_bgObj = nullptr;	// victory or defeat window.
		Image* m_img = nullptr;					// fade img
		Image* m_skillimg = nullptr;			
		Image* m_nextRetryImg = nullptr;
		Text* m_text = nullptr;
		
		Image* m_mainbuttonImg = nullptr;
		Image* m_exitbuttonImg = nullptr;
		
		bool setFinish = false;
		bool isComplete = false;
		bool isVictoryObj = false;	
		float endAlpha = 0.95f;
		float startalpha = 0.15f;
		float fadeSpeed = 0.5f;
};

