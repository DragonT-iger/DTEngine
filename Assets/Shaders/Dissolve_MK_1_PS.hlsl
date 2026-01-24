#include "Resource.hlsli"

SamplerState g_Sampler : register(s0);

struct PS_INPUT
{
    float4 PosH : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_TARGET
{

    float noise = g_NoiseMap.Sample(g_Sampler, input.UV).r;

    clip(noise - progress);

 
    float4 albedo = g_DiffuseMap.Sample(g_Sampler, input.UV);
    
    albedo *= edgeColor;

    return albedo;
}