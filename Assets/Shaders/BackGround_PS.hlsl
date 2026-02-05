

#include "Resource.hlsli"

SamplerState g_Sampler : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target
{    
    float4 Color = SkyBox_Color;
    
    Color = g_SphereMap.Sample(g_Sampler, input.uv);
    
    return Color;
}