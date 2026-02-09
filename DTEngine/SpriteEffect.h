#pragma once
#include "Effect.h"
class Texture;

class SpriteEffect : public Effect
{
	DTGENERATED_BODY(SpriteEffect);


public:
	SpriteEffect() = default;
	~SpriteEffect() = default;
public:
	void Update(float dTime) override; //
public:
	void SetSpriteSheet(uint64_t id);
	void SetFrameInterpolation(float Time) { m_frameInterpolation = Time; }
	void BindWrapped();

	void SetPlay(bool onoff); 
	void SetLoop(bool onoff) { m_loop = onoff; }
	void ResetToFirstFrame();


	void Play() { m_play = true; m_finished = false; }
	void Stop() { m_play = false; }
	void Restart() { m_currentTime = 0.f; m_play = true; m_finished = false; }
	bool IsPlaying() const { return m_play; }
	bool IsFinished() const { return m_finished; }

private:

	Vector4 m_uvTrasnform{0,0,0,0};

	std::string m_spriteName = "";
	uint64_t m_spriteID =0;
	Texture* m_texture = nullptr;

	//Sprite Frame 갯수, 
	int m_cols = 0;
	int m_rows = 0;
\
	float m_currentTime = 0;
	float m_frameInterpolation = 0.1f; //프레임간 간격 시간


	bool m_play = false;
	bool m_loop = false;
	bool  m_finished = false; 
	bool m_startedOnce = false;
};