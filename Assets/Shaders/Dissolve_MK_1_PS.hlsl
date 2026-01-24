#include "Resource.hlsli"

SamplerState g_Sampler : register(s0);

struct PS_INPUT
{
    float4 PosH : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_TARGET
{
    // 1. 노이즈 대비 강화 (과감하게 1.5제곱 정도 가해 대비를 높입니다)
    float noise = g_NoiseMap.Sample(g_Sampler, input.UV).r;
    noise = pow(noise, 1.2f);

    // 2. 마스크 설정
    float baseMask = noise - progress;
    // edgeWidth는 에디터에서 0.1 ~ 0.2 정도로 크게 설정해보세요.
    float outlineMask = noise - (progress - edgeWidth);

    // 3. 렌더링 영역 결정
    clip(outlineMask);

    float4 albedo = g_DiffuseMap.Sample(g_Sampler, input.UV);

    // 4. 층(Layer) 세분화 로직
    if (baseMask < 0)
    {
        // 0(외곽 끝) ~ 1(본체와 맞닿은 곳) 사이의 비율
        float edgeFactor = saturate(1.0f - (abs(baseMask) / edgeWidth));
        
        // [층 1] 가장 안쪽 (가장 밝게 빛나는 부분)
        if (edgeFactor > 0.7f)
        {
            float intensity = pow(edgeFactor, 4.0f) * glowIntensity * 2.0f;
            albedo.rgb = edgeColor.rgb * intensity + float3(0.3f, 0.3f, 0.3f); // 백색광 살짝 추가
        }
        // [층 2] 중간 (전형적인 타오르는 색)
        else if (edgeFactor > 0.3f)
        {
            albedo.rgb = edgeColor.rgb * glowIntensity;
        }
        // [층 3] 가장 바깥쪽 (검게 그을린 느낌)
        else
        {
            albedo.rgb = float3(0.00f, 0.00f, 0.0f); // 아주 어두운 갈색/검정
        }
    }

    return albedo;
}