#include "pch.h" // 프로젝트 환경에 맞게 수정 (없으면 제거)
#include "SoundManager.h"
#include "ResourceManager.h"
#include "Sound.h"

bool SoundManager::Initalize()
{
    if (FMOD::System_Create(&mSystem) != FMOD_OK) return false;

    if (mSystem->init(512, FMOD_INIT_NORMAL, nullptr) != FMOD_OK) return false;

    mSystem->getMasterChannelGroup(&mMasterGroup);
    mSystem->createChannelGroup("BGM", &mBGMGroup);
    mSystem->createChannelGroup("SFX", &mSFXGroup);

    mMasterGroup->addGroup(mBGMGroup);
    mMasterGroup->addGroup(mSFXGroup);

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

    // 2. 현재 BGM 페이드 인
    if (mCurrentBGM)
    {
        float vol;
        mCurrentBGM->getVolume(&vol);
        if (vol < mTargetBGMVolume) {
            vol += mFadeSpeed * deltaTime;
            if (vol > mTargetBGMVolume) vol = mTargetBGMVolume;
            mCurrentBGM->setVolume(vol);
        }
    }
}

void SoundManager::PlayBGM(const std::string& path, float volume, bool crossfade)
{
    Sound* sound = GetSound(path);
    if (!sound) return;

    mTargetBGMVolume = Clamp01(volume);

    // 기존 재생 중인 BGM이 있다면 Previous로 밀어냄
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
    native->setMode(FMOD_LOOP_NORMAL); // BGM은 기본 루프

    // 새 BGM 재생 (BGM 그룹에 할당)
    mSystem->playSound(native, mBGMGroup, false, &mCurrentBGM);

    // 크로스페이드 사용 시 0부터 시작, 아니면 바로 목표 볼륨
    mCurrentBGM->setVolume(crossfade ? 0.0f : mTargetBGMVolume);
}

void SoundManager::PlaySFX(const std::string& path, float volume)
{
    Sound* sound = GetSound(path);
    if (!sound) return;

    FMOD::Sound* native = sound->GetNative();
    native->setMode(FMOD_LOOP_OFF); // 효과음은 루프 안함

    FMOD::Channel* ch = nullptr;
    // SFX 그룹에 할당하여 재생 (채널 관리는 FMOD가 알아서 함)
    mSystem->playSound(native, mSFXGroup, false, &ch);

    if (ch) {
        ch->setVolume(Clamp01(volume));
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
    if (mMasterGroup) mMasterGroup->setVolume(Clamp01(volume));
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

void SoundManager::PlayOneShot(const std::string& path, float volume)
{
    Sound* sound = GetSound(path);
    if (!sound) return;

    FMOD::Sound* native = sound->GetNative();
    native->setMode(FMOD_LOOP_OFF); // 루프 끔

    FMOD::Channel* ch = nullptr;
    mSystem->playSound(native, mSFXGroup, false, &ch);
    if (ch) ch->setVolume(Clamp01(volume));
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
