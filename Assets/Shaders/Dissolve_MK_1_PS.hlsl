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
    noise = pow(noise, 1.2f);

    // 2. 마스크 설정
    float baseMask = noise - progress;
    float outlineMask = noise - (progress - edgeWidth);

    clip(outlineMask);

    float4 albedo = g_DiffuseMap.Sample(g_Sampler, input.UV);

    if (baseMask < 0)
    {
        float edgeFactor = saturate(1.0f - (abs(baseMask) / edgeWidth));
        
        if (edgeFactor > 0.7f)
        {
            float intensity = pow(edgeFactor, 4.0f) * glowIntensity * 2.0f;
            albedo.rgb = edgeColor.rgb * intensity + float3(0.3f, 0.3f, 0.3f); // 백색광 살짝 추가
        }
        else if (edgeFactor > 0.3f)
        {
            albedo.rgb = edgeColor.rgb * glowIntensity;
        }
        else
        {
            albedo.rgb = float3(0.00f, 0.00f, 0.0f); 
        }
    }

    return albedo;
}