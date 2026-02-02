// Skeletal_PS.hlsl 수정
#include "Lighting.hlsli"
#include "Resource.hlsli"




struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
    float3 WorldPos : POSITION;
    float3 WorldNormal : NORMAL;
    float4 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    
};
SamplerState g_Sampler : register(s0);


float3 ApplyHeightFog(float3 finalColor, float3 worldPos, float3 cameraPos)
{
    float3 viewDir = worldPos - cameraPos;
    float dist = length(viewDir);
    float3 unitDir = viewDir / dist;

    // 안개가 시작되는 거리(StartDistance) 보정 [cite: 19, 93]
    float modifiedDist = max(dist - FogStartDistance, 0.0f);
    
    // 현재 픽셀의 상대적 높이 계산 (기준 높이 적용)
    float relativeCameraY = cameraPos.y - FogBaseHeight;
    
    // 수평을 바라볼 때 분모가 0이 되는 것을 방지
    float slope = unitDir.y;
    if (abs(slope) < 0.0001f)
        slope = 0.0001f;

    // 높이 기반 지수 안개 적분 공식
    // exp(-relativeCameraY * FogHeightFalloff)는 카메라 높이에서의 밀도
    // (1.0 - exp(-modifiedDist * slope * FogHeightFalloff)) / (slope * FogHeightFalloff)는 경로 적분 결과
    float fogAmount = (FogGlobalDensity / FogHeightFalloff) * exp(-relativeCameraY * FogHeightFalloff) * (1.0f - exp(-modifiedDist * slope * FogHeightFalloff)) / slope;

    float fogFactor = saturate(fogAmount);
    
    
    float4 Color = SkyBox_Color; //배경색
    
    return lerp(finalColor, Color.rgb, fogFactor);
}

float4 PS(PS_INPUT input) : SV_Target
{

    float3 finalColor = g_DiffuseMap.Sample(g_Sampler, input.UV);
    
    finalColor = ApplyHeightFog(finalColor, input.WorldPos, CameraPos);
    
    return float4(finalColor, 1.0f);
}