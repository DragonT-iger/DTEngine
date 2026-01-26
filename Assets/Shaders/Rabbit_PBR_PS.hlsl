#include "PBR_Shared.hlsli"

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

float4 PS(PS_INPUT input) : SV_Target
{
    float3 N = normalize(input.WorldNormal);
    float3 V = (IsOrtho > 0.5f) ? -normalize(CameraDir) : normalize(CameraPos - input.WorldPos);
    
    float3 albedo = float3(1.0f, 1.0f, 1.0f);
    float metal = 0.0f;
    float rough = 1.0f;
    float ao = 1.0f;

    if (USE_NORMAL)
    {
        float4 texNormal = g_NormalMap.Sample(g_Sampler, input.UV);
        N = GetWorldNormalFromNormalMap(texNormal, N, input.Tangent, input.Bitangent);
    }

    if (USE_METAL)
        metal = g_MetalMap.Sample(g_Sampler, input.UV).r ;
    if (USE_ROUGH)
        rough = g_RoughMap.Sample(g_Sampler, input.UV).r * Roughness_Factor;
    if (USE_AO)
        ao = g_AoMap.Sample(g_Sampler, input.UV).r;
    
    if (USE_ALBEDO)
    {
        float4 texBase = g_DiffuseMap.Sample(g_Sampler, input.UV);
        albedo = texBase.rgb;
    }

    float3 directLighting = float3(0, 0, 0);
    float shadowFactor = CalculateShadow(input.WorldPos, Shadow_Bias);
    
    
    float Temp = 1;
    
    for (int i = 0; i < ActiveCount; ++i)
    {
        float3 L;
        float attenuation = 1.0f;
        float type = Lights[i].DirectionType.w;

        // 조명 타입별 방향 및 감쇠 연산
        if (type < 0.5f) // Directional Light
        {
            L = normalize(Lights[i].DirectionType.xyz);
            attenuation = 1.0f;
        }
        else // Point Light
        {
            float3 toLight = Lights[i].PositionRange.xyz - input.WorldPos;
            float dist = length(toLight);
            L = normalize(toLight);
            
            float range = Lights[i].PositionRange.w;
            // 거리 기반 감쇠 (Lighting.hlsli 공식 적용)
            attenuation = saturate(1.0 / (1.0 + 0.1 * dist / range + 0.01 * dist / range * dist / range));
        }

        // 첫 번째 조명에만 그림자 적용 (Shadow Factor)
        float currentShadow = (i == 0) ? Temp = shadowFactor *0.6f : 1.0f;
        
        // DisneyPBR (Shared 함수) 호출하여 누적
        // 최종 광원 세기에 감쇠와 그림자 인자를 통합하여 전달
        directLighting += DisneyPBR(
            input.WorldPos,
            N,
            V,
            albedo,
            rough,
            metal,
            L,
            Lights[i].ColorIntensity.rgb,
            Lights[i].ColorIntensity.w * attenuation// * currentShadow
        );
        
        
        directLighting *= currentShadow;

    }
    
   // directLighting *= Shadow_Scale;
        
    float3 ambientLighting = albedo * 0.7f;
    
    float3 finalColor = directLighting + ambientLighting;
    
    float alpha = USE_ALBEDO ? g_DiffuseMap.Sample(g_Sampler, input.UV).a : 1.0f;
    return float4(finalColor, alpha);
}