#pragma once
#include "Singleton.h"
#include <string>
#include <vector>
#include <map>

class Sound;

namespace FMOD
{
    class System;
    class Channel;
    class ChannelGroup;
}

class SoundManager : public Singleton<SoundManager>
{

    friend class Singleton<SoundManager>;

public:

    bool Initalize();
    void Update(float deltaTime); // 페이드 처리를 위해 매 프레임 호출 필수
    void Release();

    // BGM: 하나만 재생되며 교체 시 크로스페이드
    void PlayBGM(const std::string& path,  bool crossfade = true);
    void StopBGM(bool fadeOut = true);

    // SFX: 여러 개가 동시에 재생됨
    void PlaySFX(const std::string& path);

    // 볼륨 제어
    void SetMasterVolume(float volume);
    void SetBGMVolume(float volume);
    void SetSFXVolume(float volume);

   

    void PlayOneShot(const std::string& path);

    FMOD::Channel* PlayLoop(const std::string& path, float volume = 1.0f);
    FMOD::System* GetSystem() { return mSystem; }

    void ShutDown();

    bool PlaySceneBGM(const std::string& SceneName, bool crossfaded = true);

private:
    SoundManager() = default;
    ~SoundManager();

    float Clamp01(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }
    Sound* GetSound(const std::string& path);

    bool Crosschecked(std::string Song);

    std::map<std::string, std::string> m_BGMMap; //Scene Start때;
    std::map<std::string, bool> m_havePlatyed;

private:
    FMOD::System* mSystem = nullptr;
    FMOD::ChannelGroup* mMasterGroup = nullptr;
    FMOD::ChannelGroup* mBGMGroup = nullptr;
    FMOD::ChannelGroup* mSFXGroup = nullptr;

    // BGM 트랜지션을 위한 채널
    FMOD::Channel* mCurrentBGM = nullptr;
    FMOD::Channel* mPreviousBGM = nullptr;

    float mTargetBGMVolume = 0.5f;
    float mTargetSFXVolume = 0.5f;
    float mFadeSpeed = 0.4f; // 초당 페이드 속도 (조절 가능)
    std::string m_On_Played = "";
}; 