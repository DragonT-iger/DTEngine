#include "Resource.hlsli"

SamplerState Sampler_Default : register(s0);

float4 PS(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET
{
    float4 color = g_DiffuseMap.Sample(Sampler_Default, uv);

    color.rgb *= MaterialColor.rgb;
   
    clip(color.a - 0.01f);
    
    return color;
}

