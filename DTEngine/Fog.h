#pragma once
#include "MonoBehaviour.h"

class Fog : public MonoBehaviour
{
	DTGENERATED_BODY(Fog);

public:
	Fog() = default;
	~Fog() = default;
public:
	void Start() override;

public:
	void BindFog();

	//색상은 일단 하얀색으로 통일.

	void SetFogDensity(float Den) { m_FogGlobalDensity = Den; }//말 그대로 계수
	void SetFogFallOff(float Val) { m_FogHeightFalloff = Val; }//감쇄계수
	void SetFogHeight(float Height) { m_FogBaseHeight = Height; }//안개 시작하는 높이
	void SetFogDistance(float Dis) { m_FogStartDistance = Dis; }//안개 시작하는 거리 

private:
	 float m_FogGlobalDensity = 0.2f;
	 float m_FogHeightFalloff = 0.7f;
	 float m_FogBaseHeight = 10.f;
	 float m_FogStartDistance = 10.f;
};