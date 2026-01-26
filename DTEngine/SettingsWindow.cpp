#include "pch.h"
#include "SettingsWindow.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Image.h"
#include "UIButton.h"
#include "UISlider.h"

BEGINPROPERTY(SettingsWindow)
DTPROPERTY_ACCESSOR(SettingsWindow, m_bgmVolume, GetBGMVolume, SetBGMVolume)
DTPROPERTY_ACCESSOR(SettingsWindow, m_sfxVolume, GetSFXVolume, SetSFXVolume)
ENDPROPERTY()

void SettingsWindow::Awake()
{
		CreateOnce();
}

void SettingsWindow::CreateOnce()
{
		if (m_created) return;
		InitializeWindow();
		if (m_settingWindow) m_settingWindow->SetActive(false);
		m_created = true;
}

void SettingsWindow::Open()
{
		if (!m_settingWindow) return;

		m_settingWindow->SetActive(true);
}

void SettingsWindow::Close()
{
		if (!m_settingWindow) return;

		m_settingWindow->SetActive(false);
}

void SettingsWindow::InitializeWindow()
{
		Scene* scene = SceneManager::Instance().GetActiveScene();

		// window 생성.
		GameObject* settingWindow = scene->CreateGameObject("SettingWindow");
		m_settingWindow = settingWindow;

		// window BG.
		GameObject* settingWindowBG = scene->CreateGameObject("SettingWindowBG");
		settingWindowBG->GetTransform()->SetParent(settingWindow->GetTransform());
		settingWindowBG->AddComponent<Image>();

		// titlebar. 
		GameObject* titleBar = scene->CreateGameObject("TitleBar");
		titleBar->GetTransform()->SetParent(settingWindowBG->GetTransform());
		titleBar->AddComponent<Image>();

		// bgm slider
		GameObject* bgmslider = scene->CreateGameObject("Bgmslider");
		bgmslider->GetTransform()->SetParent(settingWindowBG->GetTransform());
		bgmslider->AddComponent<Image>();
		bgmslider->AddComponent<UISlider>();
		m_BGMSlider = bgmslider->GetComponent<UISlider>();

		// sfx slider
		GameObject* sfxslider = scene->CreateGameObject("Sfxslider");
		sfxslider->GetTransform()->SetParent(settingWindowBG->GetTransform());
		sfxslider->AddComponent<Image>();
		sfxslider->AddComponent<UISlider>();
		m_SFXSlider = sfxslider->GetComponent<UISlider>();

		// return to main button
		GameObject* returnButton = scene->CreateGameObject("ReturnButton");
		returnButton->GetTransform()->SetParent(settingWindowBG->GetTransform());
		returnButton->AddComponent<UIButton>();
		returnButton->GetComponent<UIButton>()->SetOnClick([this]()
				{
						// 여기에 main scene으로 돌아가기 넣어야함. 상세한거 아직 없어서 이후 추가 예정.
						Close();
				}
		);

		// exit button
		GameObject* exitButton = scene->CreateGameObject("ExitButton");
		exitButton->GetTransform()->SetParent(settingWindowBG->GetTransform());
		exitButton->AddComponent<UIButton>();
		exitButton->GetComponent<UIButton>()->SetOnClick([this]()
				{
						Close();
				}
		);

		// settingButton 생성.
		GameObject* settingToggleButton = scene->CreateGameObject("SettingToggleButton");
		settingToggleButton->AddComponent<Image>();
		settingToggleButton->AddComponent<UIButton>();
		m_settingButtonObj = settingToggleButton;

		// 람다로 내 오브젝트 넣어주기.
		m_settingButtonObj->GetComponent<UIButton>()->SetOnClick([this]()
				{
						// toggle 해주기.
						m_settingWindow->SetActive(!m_settingWindow->IsActive());
				});
}

void SettingsWindow::SetBGMVolume(float val)
{
		m_bgmVolume = val;
		if (m_BGMSlider) m_BGMSlider->SetValue(val);
}

void SettingsWindow::SetSFXVolume(float val)
{
		m_sfxVolume = val;
		if (m_SFXSlider) m_SFXSlider->SetValue(val);
}
