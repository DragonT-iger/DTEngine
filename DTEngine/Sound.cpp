#include "pch.h"
#include "Sound.h"
#include "SoundManager.h"

#include <fmod.hpp>

Sound::Sound()
{

}

Sound::~Sound()
{
    if (mSound && SoundManager::Instance().GetSystem() != nullptr)
    {
        mSound->release();
        mSound = nullptr;
    }
}

// 직렬화랑 연동을 시킬거면, 문자열의 이름을 멤버로 갖고. SoundComponent에 적용시키는 형식으로 진행해야 할 거 같음.
// SoundComponent에서는 


bool Sound::LoadFile(const std::string& fullPath)
{

    FMOD::System* fmodSystem = SoundManager::Instance().GetSystem();

    if (!fmodSystem) return false;

    // 2. FMOD API를 통해 파일을 로드하여 mSound(FMOD::Sound*)에 할당합니다.
    // FMOD_DEFAULT는 소리를 메모리에 전부 올리는 방식입니다. (효과음용)

    FMOD_RESULT result = fmodSystem->createSound(
        fullPath.c_str(),
        FMOD_DEFAULT,
        nullptr,
        &mSound
    );

    return (result == FMOD_OK);



    return false;
}

bool Sound::SaveFile(const std::string& fullPath)
{
    return false;
}

void Sound::Unload()
{

}
