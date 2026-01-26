#pragma once
#include "SimpleMathHelper.h"
#include "MonoBehaviour.h"

class UISlider;
class GameObject;

class SettingsWindow : public MonoBehaviour
{
    DTGENERATED_BODY(SettingsWindow);
public:
    SettingsWindow() = default;
    ~SettingsWindow() override = default;

    void Awake() override;
    // 필요 시 추가 예정. 지금은 생성만 하고 관리하기.
    //void Update(float deltaTime) override;

    void CreateOnce();   // InitializeWindow
    void Open();         // 창 켜고 선택 초기화
    void Close();        // 창 끄기

    void InitializeWindow();


    // 설정 window에서 편하게 가져가게할려고.
    UISlider* GetBGMSlider() { return m_BGMSlider; }

    UISlider* GetSFXSlider() { return m_SFXSlider; }

    void SetBGMVolume(float val);
    const float& GetBGMVolume() const { return m_bgmVolume; }

    void SetSFXVolume(float val);
    const float& GetSFXVolume() const { return m_sfxVolume; }
private:
    bool m_created = false;

    GameObject* m_settingWindow = nullptr;
    GameObject* m_settingButtonObj = nullptr;

    UISlider* m_BGMSlider = nullptr;
    UISlider* m_SFXSlider = nullptr;

    float m_bgmVolume = 1.0f;
    float m_sfxVolume = 1.0f;
};

