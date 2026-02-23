#include "PBR_Shared.hlsli"

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
    float3 WorldPos : POSITION;
    float3 WorldNormal : NORMAL;
    float4 Tangent: TANGENT;
    float3 Bitangent : BITANGENT;
};

SamplerState g_Sampler : register(s0);

float4 PS(PS_INPUT input) : SV_Target
{
    float3 N = normalize(input.WorldNormal);
    float3 V;
    
    if (IsOrtho > 0.5f) // 0일때 ortho
    {
        V = -normalize(CameraDir); // 엔진에서 - 안해줘도 됨 어짜피 어셈블리 열어보면 자동으로 mad 처리 됨
    }
    else
    {
        V = normalize(CameraPos - input.WorldPos);
    }
    //V = -normalize(CameraDir);
    float3 albedo = float3(1.0f, 1.0f, 1.0f);
    float metal = 0.0f;
    float rough = 1.0f;
    float ao = 1.0f;
    
     
    
    if (USE_NORMAL)
    {
        float4 texNormal = g_NormalMap.Sample(g_Sampler, input.UV);
        N = GetWorldNormalFromNormalMap(texNormal, N, input.Tangent , input.Bitangent);
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
    
    float shadowFactor = CalculateShadow(input.WorldPos , Shadow_Bias);

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
            attenuation = saturate(1.0 / (1.0 + 0.1 * dist / range + 0.01 * dist / range * dist / range));
        }

        float currentShadow = (i == 0) ? shadowFactor : 1.0f;

        
        directLighting += DisneyPBR(
            input.WorldPos,
            N,
            V,
            albedo,
            rough,
            metal,
            L,
            Lights[i].ColorIntensity.rgb,
            Lights[i].ColorIntensity.w * attenuation * currentShadow
        );
    }

    float3 ambientLighting = float3(0, 0, 0);
    if (USE_IBL)
    {
        float3 R = reflect(-V, N);
        float mipLevel = rough * 7.0f; // Roughness 기반 밉맵 샘플링
        
        // Specular 및 Diffuse 환경광 샘플링
        float3 specEnv = g_CubeMap.SampleLevel(g_Sampler, R, mipLevel).rgb;
        float3 diffEnv = g_CubeMap.SampleLevel(g_Sampler, N, 7.0f).rgb;

        ambientLighting = CalculateIBL_Combined(specEnv, diffEnv, V, N, albedo, rough, metal, ao);
    }

    float3 finalColor = directLighting + ambientLighting;

        
    
    return float4(finalColor, 1.0f);
}