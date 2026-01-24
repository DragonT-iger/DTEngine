#include "Resource.hlsli"

SamplerState g_Sampler : register(s0);

struct PS_INPUT
{
    float4 PosH : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_TARGET
{

    float2 animatedUV = input.UV * noiseScalef;
    animatedUV += (totalTime * 0.1f); 
    
    float noise = g_NoiseMap.Sample(g_Sampler, animatedUV).r;

  
    clip(noise - progress);

    // [3] 기본 UI 텍스처 샘플링
    float4 albedo = float4(1, 1, 1, 1);
    if (USE_ALBEDO) 
    {
        albedo = g_DiffuseMap.Sample(g_Sampler, input.UV);
    }
    
    // UI의 Vertex Color나 Material Color 곱하기 (투명도 포함)
    albedo *= MaterialColor;

    // [4] Burning Edge (테두리 발광) 효과
    // progress와 (progress + edgeWidth) 사이의 영역을 추출 
    float edgeFactor = smoothstep(progress, progress + edgeWidth, noise);
    
    // edgeFactor가 0에 가까울수록(경계선에 가까울수록) 발광 색상이 강해짐
    // edgeColor와 glowIntensity를 곱해 HDR 느낌의 빛을 만듦 
    float3 fireGlow = (1.0f - edgeFactor) * edgeColor.rgb * glowIntensity;

    // [5] 최종 합성
    // 원본 색상에 발광 효과를 더함. UI이므로 알파값은 원본 유지.
    return float4(albedo.rgb + fireGlow, albedo.a);
}