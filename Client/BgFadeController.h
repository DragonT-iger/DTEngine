#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class GameObject;
class Image;
class Text;
class UIButton;

enum class FadeType { Victory, Defeat, GameOver };

class BgFadeController : public MonoBehaviour
{
		DTGENERATED_BODY(BgFadeController);
public:
		void Start() override;
		void Update(float deltaTime) override;
		void BGFadeUpdate(float deltaTime);

		void Init();
		void SetFadeType(FadeType type) { m_fadeType = type; }
		void SetVictoryObj(bool value);
		void SetDefeatObj(bool value);
		void SetGameOverObj(bool value);

		void SettingToggleFinish();
		// int에 따라서 둘 중 한개 켜주기.
		bool IsMushroomSkillSet();
private:
		FadeType m_fadeType = FadeType::Victory;

		GameObject* m_settingButton = nullptr;
		GameObject* m_rayObj = nullptr;
		GameObject* m_bgObj = nullptr;	// victory or defeat window.
		
		Image* m_img = nullptr;					// fade img
		Image* m_mushroomSkillimg = nullptr;			
		Image* m_bottleSkillimg = nullptr;
		Image* m_lifeImg = nullptr;
		Image* m_nextRetryImg = nullptr;
		Text* m_text = nullptr;
		
		Image* m_mainbuttonImg = nullptr;
		Image* m_exitbuttonImg = nullptr;
		
		GameObject* m_gameOverObj = nullptr;
		Image* m_gameOverMainBtnImg = nullptr;

		bool setFinish = false;
		bool isComplete = false;
		bool isVictoryObj = false;	
		float endAlpha = 0.95f;
		float startalpha = 0.15f;
		float fadeSpeed = 0.5f;
		float delayTime = 0.5f;
		float time = 0;
};

