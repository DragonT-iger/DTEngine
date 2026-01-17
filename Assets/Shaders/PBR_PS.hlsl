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
    // [1] 데이터 초기화 및 기본값 설정
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
    V = -normalize(CameraDir);
    float3 albedo = float3(1.0f, 1.0f, 1.0f);
    float metal = 0.0f;
    float rough = 1.0f;
    float ao = 1.0f;
    
     
    
    // [2] 플래그 기반 텍스처 샘플링 및 데이터 업데이트
    // Normal Map 적용
    if (USE_NORMAL)
    {
        float4 texNormal = g_NormalMap.Sample(g_Sampler, input.UV);
        N = GetWorldNormalFromNormalMap(texNormal, N, input.Tangent , input.Bitangent);
    }

    // PBR 속성 적용
    if (USE_METAL)
        metal = g_MetalMap.Sample(g_Sampler, input.UV).r * Metallic_Factor;
    if (USE_ROUGH)
        rough = g_RoughMap.Sample(g_Sampler, input.UV).r * Roughness_Factor;
    if (USE_AO)
        ao = g_AoMap.Sample(g_Sampler, input.UV).r;
    
    // Albedo 적용 (AO를 미리 곱함)
    if (USE_ALBEDO)
    {
        float4 texBase = g_DiffuseMap.Sample(g_Sampler, input.UV);
        albedo = texBase.rgb;
    }

    // [3] 직접광(Direct Lighting) 계산 - 루프 내부에 로직 인라이닝
    float3 directLighting = float3(0, 0, 0);
    
    // 첫 번째 조명(주광원)에 대한 그림자 계산
    float shadowFactor = CalculateShadow(input.WorldPos);

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
        float currentShadow = (i == 0) ? shadowFactor : 1.0f;

        // DisneyPBR (Shared 함수) 호출하여 누적
        // 최종 광원 세기에 감쇠와 그림자 인자를 통합하여 전달
        directLighting += DisneyPBR(
            input.WorldPos,
            N,
            albedo,
            rough,
            metal,
            L,
            Lights[i].ColorIntensity.rgb,
            Lights[i].ColorIntensity.w * attenuation * currentShadow
        );
    }

    // [4] 간접광(IBL) 계산 - Shared 함수 활용
    float3 ambientLighting = float3(0, 0, 0);
    if (USE_IBL)
    {
        float3 R = reflect(-V, N);
        float mipLevel = rough * 7.0f; // Roughness 기반 밉맵 샘플링
        
        // Specular 및 Diffuse 환경광 샘플링
        float3 specEnv = g_CubeMap.SampleLevel(g_Sampler, R, mipLevel).rgb;
        float3 diffEnv = g_CubeMap.SampleLevel(g_Sampler, N, 7.0f).rgb;

        // 물리 연산은 Shared 함수 호출 (데이터만 전달)
        ambientLighting = CalculateIBL_Combined(specEnv, diffEnv, V, N, albedo, rough, metal, ao);
    }

    // [5] 최종 결과 합성 및 감마 보정
    float3 finalColor = directLighting + ambientLighting *0.5f;

    finalColor = ACESToneMapping(finalColor);
    
    return float4(finalColor, 1.0f);
}