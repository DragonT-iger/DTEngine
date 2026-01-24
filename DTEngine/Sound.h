#pragma once
#include "IResource.h"
namespace FMOD { class Sound; } //전방선언

class Sound : public IResource
{

public:
	Sound();
	virtual ~Sound();

	bool LoadFile(const std::string& fullPath) override;
	bool SaveFile(const std::string& fullPath) override;
	void Unload() override;


	FMOD::Sound* GetNative() const { return mSound; }
private:

	FMOD::Sound* mSound = nullptr; 
//	uint16_t m_SoundID = 0;
	std::string m_Name; //디버깅
};