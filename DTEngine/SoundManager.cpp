#include "pch.h" // 프로젝트 환경에 맞게 수정 (없으면 제거)
#include "SoundManager.h"
#include "ResourceManager.h"
#include "Sound.h"
#include <fmod.hpp>



bool SoundManager::Initalize()
{
    if (FMOD::System_Create(&mSystem) != FMOD_OK) return false;

    if (mSystem->init(512, FMOD_INIT_NORMAL, nullptr) != FMOD_OK) return false;

    mSystem->getMasterChannelGroup(&mMasterGroup);
    mSystem->createChannelGroup("BGM", &mBGMGroup);
    mSystem->createChannelGroup("SFX", &mSFXGroup);

    mMasterGroup->addGroup(mBGMGroup);
    mMasterGroup->addGroup(mSFXGroup);


    m_BGMMap.emplace("TitleScene", "BGM/Title");
    m_BGMMap.emplace("TutorialScene", "BGM/Queen's Theme");
    m_BGMMap.emplace("BossDialogue", "BGM/Cheshire's Theme");
    //m_BGMMap.emplace("EndingScene", "BGM/Cheshire's Theme");
    //m_BGMMap.emplace("Stage_01", "The_World Is_Yours");
    //m_BGMMap.emplace("Stage_02", "Doomsday");
    //m_BGMMap.emplace("Stage_03", "Doomsday"); 
    //m_BGMMap.emplace("MainGameScene", "BGM/BGM_Setting");
    //m_BGMMap.emplace("GridTestSceneHS2", "BGM/BGM_Setting");
    //m_BGMMap.emplace("GridTestSceneHS2", "BGM/Boss_Setting");

    return true;
}

void SoundManager::Update(float deltaTime)
{
    if (!mSystem) return;

    mSystem->update();

    // --- BGM 크로스페이드 처리 ---

    // 1. 이전 BGM 페이드 아웃 및 정지
    if (mPreviousBGM)
    {
        float vol;
        mPreviousBGM->getVolume(&vol);
        vol -= mFadeSpeed * deltaTime;

        if (vol <= 0.0f) {
            mPreviousBGM->stop();
            mPreviousBGM = nullptr;
        }
        else {
            mPreviousBGM->setVolume(vol);
        }
    }

    if (mCurrentBGM)
    {
        float vol;
        mCurrentBGM->getVolume(&vol);

        if (mPreviousBGM == nullptr)
        {
            if (std::abs(vol - mTargetBGMVolume) > 0.001f)
            {
                if (vol < mTargetBGMVolume)
                    vol = std::min(vol + mFadeSpeed * deltaTime, mTargetBGMVolume);
                else
                    vol = std::max(vol - mFadeSpeed * deltaTime, mTargetBGMVolume);

                mCurrentBGM->setVolume(vol);
            }
        }
        else
        {
            if (vol < mTargetBGMVolume) {
                vol += mFadeSpeed * deltaTime;
                if (vol > mTargetBGMVolume) vol = mTargetBGMVolume;
                mCurrentBGM->setVolume(vol);
            }
        }
    }
}

void SoundManager::PlayBGM(const std::string& path, bool crossfade)
{

    std::string Path = "Assets/Sound/" + path + ".mp3";

    Sound* sound = GetSound(Path);
    if (!sound) return;

    if (mPreviousBGM)
    {
        mPreviousBGM->stop(); 
        mPreviousBGM = nullptr;
    }

    if (mCurrentBGM)
    {
        if (crossfade) {
            mPreviousBGM = mCurrentBGM;
        }
        else {
            mCurrentBGM->stop();
        }
    }

    FMOD::Sound* native = sound->GetNative();
    native->setMode(FMOD_LOOP_NORMAL);

    mSystem->playSound(native, mBGMGroup, false, &mCurrentBGM);

    mCurrentBGM->setVolume(crossfade ? 0.0f : mTargetBGMVolume);

    m_On_Played = path;
}

void SoundManager::PlaySFX(const std::string& path)
{
    Sound* sound = GetSound(path);
    if (!sound) return;

    FMOD::Sound* native = sound->GetNative();
    native->setMode(FMOD_LOOP_OFF); // 효과음은 루프 안함

    FMOD::Channel* ch = nullptr;
    // SFX 그룹에 할당하여 재생 (채널 관리는 FMOD가 알아서 함)
    mSystem->playSound(native, mSFXGroup, false, &ch);

    if (ch) {
        ch->setVolume(mTargetSFXVolume);
    }
}

void SoundManager::StopBGM(bool fadeOut)
{
    if (!mCurrentBGM) return;

    if (fadeOut) {
        mPreviousBGM = mCurrentBGM;
        mCurrentBGM = nullptr;
    }
    else {
        mCurrentBGM->stop();
        mCurrentBGM = nullptr;
    }
}

void SoundManager::SetMasterVolume(float volume)
{
    //if (mMasterGroup) mMasterGroup->setVolume(Clamp01(volume));
}

void SoundManager::SetBGMVolume(float volume)
{
    if (mBGMGroup) mBGMGroup->setVolume(Clamp01(volume));



}

void SoundManager::SetSFXVolume(float volume)
{
    if (mSFXGroup) mSFXGroup->setVolume(Clamp01(volume));
}

SoundManager::~SoundManager()
{
    Release();
}

Sound* SoundManager::GetSound(const std::string& path)
{
    return ResourceManager::Instance().Load<Sound>(path);
}

bool SoundManager::Crosschecked(std::string SceneName)
{
    std::string Name = "";

    if (auto it = m_BGMMap.find(SceneName); it != m_BGMMap.end())
    {
        Name = it->second;
    }
    else
        return false;

    if (Name == m_On_Played) return false; //겹치는 경우 crossfade 안함. 매 프레임마다 start 호출해서 최적화 하려고.
    return true;


}

void SoundManager::Release()
{
    if (mSystem)
    {
        // 1. 모든 채널의 소리를 즉시 정지 (Master 그룹을 정지하면 하위 BGM, SFX 모두 정지됨)
        if (mMasterGroup)
        {
            mMasterGroup->stop();
        }

        mCurrentBGM = nullptr;
        mPreviousBGM = nullptr;

     
        mSystem->release();
        mSystem = nullptr;
    }
}


// --- SFX Loop (특수 효과음용, 직접 정지시켜야 함) ---

void SoundManager::PlayOneShot(const std::string& path)
{

    std::string Path = "Assets/Sound/" + path + ".mp3";

    Sound* sound = GetSound(Path);
    if (!sound) return;

    FMOD::Sound* native = sound->GetNative();
    native->setMode(FMOD_LOOP_OFF); // 루프 끔

    FMOD::Channel* ch = nullptr;
    mSystem->playSound(native, mSFXGroup, false, &ch);
    if (ch) ch->setVolume(mTargetSFXVolume);
}

FMOD::Channel* SoundManager::PlayLoop(const std::string& path, float volume)
{
    Sound* sound = GetSound(path);
    if (!sound) return nullptr;

    FMOD::Sound* native = sound->GetNative();
    native->setMode(FMOD_LOOP_NORMAL); // 루프 켬

    FMOD::Channel* ch = nullptr;
    mSystem->playSound(native, mSFXGroup, false, &ch);
    if (ch) ch->setVolume(Clamp01(volume));

    return ch; // 외부에서 stop()할 수 있도록 채널 반환
}

void SoundManager::ShutDown()
{
    if (mMasterGroup)
    {
        mMasterGroup->stop();
    }
}

bool SoundManager::PlaySceneBGM(const std::string& SceneName, bool crossfaded)
{
    if (!Crosschecked(SceneName)) return false;

    std::string Name = "";
    if (auto it = m_BGMMap.find(SceneName); it != m_BGMMap.end())
    {
        Name = it->second;
    }
    else
    {
        std::cout << "Scene 전환 시, Sound처리오류남. Scene Name: -> " << SceneName << std::endl;
        return false;
    }

    std::string Path = "Assets/Sound/" + Name + ".mp3";

    Sound* sound = GetSound(Path);
    if (!sound) return false;

    if (mCurrentBGM)
    {
        if (crossfaded) {
            mPreviousBGM = mCurrentBGM;
        }
        else {
            mCurrentBGM->stop();
        }
    }

    FMOD::Sound* native = sound->GetNative();
    native->setMode(FMOD_LOOP_NORMAL);

    // 새 BGM 재생
    mSystem->playSound(native, mBGMGroup, false, &mCurrentBGM);

    mCurrentBGM->setVolume(crossfaded ? 0.0f : mTargetBGMVolume);
    m_On_Played = Name;

    return true;
}
