#include "pch.h"
#include "Fog.h"
#include "DX11Renderer.h"


BEGINPROPERTY(Fog)
DTPROPERTY(Fog, m_FogGlobalDensity, SetFogDensity)
DTPROPERTY(Fog, m_FogHeightFalloff, SetFogFallOff)
DTPROPERTY(Fog, m_FogBaseHeight, SetFogHeight)
DTPROPERTY(Fog, m_FogStartDistance, SetFogDistance)

ENDPROPERTY()



//대충 cb binding

void Fog::BindFog()
{
	FogParams data;
	data.FogBaseHeight = m_FogBaseHeight;
	data.FogHeightFalloff = m_FogHeightFalloff;
	data.FogStartDistance = m_FogStartDistance;
	data.FogGlobalDensity = m_FogGlobalDensity;


	DX11Renderer::Instance().UpdateFog_CBUFFER(data);
}

void Fog::Start()
{
}

