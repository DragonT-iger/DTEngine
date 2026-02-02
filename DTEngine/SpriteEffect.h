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
	//Time Scale은 Effect가 갖고 있는 상수버퍼 값을 재활용함. 
};