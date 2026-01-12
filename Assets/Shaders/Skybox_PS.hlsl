

#include "Resource.hlsli"

SamplerState g_Sampler : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 TexCoord : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target
{
    float4 finalColor =  g_CubeMap.Sample(g_Sampler, input.TexCoord);
    
    if (NEED_ON_GAMMA)
    {
        finalColor = pow(finalColor, 1.0f / 2.2f); // 
    }
    return finalColor;
}