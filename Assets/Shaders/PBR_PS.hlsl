#include "Lighting_PBR.hlsli"

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 WorldPos : POSITION;
<<<<<<< .merge_file_lwKyAr
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

cbuffer CBuffer_Material : register(b3)
{
    float4 MaterialColor;
    float4 UVTransform;
    int UseTexture;
    int3 Padding2;
};
=======
    float3 WorldNormal : NORMAL;
};

#define PI 3.1415926535
>>>>>>> .merge_file_oJalNe

Texture2D t_Albedo : register(t0);
Texture2D t_Metallic : register(t1);
Texture2D t_Roughness : register(t2);
Texture2D t_Normal : register(t3);
Texture2D t_AO : register(t4);

SamplerState g_Sampler : register(s0);


float3 CalculateNormal(PS_INPUT input, float2 uv)
{
    float3 normalSample = t_Normal.Sample(g_Sampler, uv).rgb;
    float3 tangentNormal = normalSample * 2.0 - 1.0;

    float3 N = normalize(input.Normal);
    float3 T = normalize(input.Tangent);
    float3 B = normalize(input.Bitangent);

    float3x3 TBN = float3x3(T, B, N);

    return normalize(mul(tangentNormal, TBN));
}



float4 PS(PS_INPUT input) : SV_Target
{
<<<<<<< .merge_file_lwKyAr
    float2 uv = input.UV * UVTransform.xy + UVTransform.zw;

    float4 albedoSample = t_Albedo.Sample(g_Sampler, uv);
    float3 albedo = (UseTexture & 1) ? albedoSample.rgb : MaterialColor.rgb;
    float alpha = (UseTexture & 1) ? albedoSample.a : MaterialColor.a;

    // Alpha Clipping
    //clip(alpha < 0.1f ? -1 : 1);

   
    float3 normal = CalculateNormal(input , uv);
    
    // 노멀맵 이따가 적용

    // UseTexture 비트 플래그를 활용해 제어한다고 가정 (예: 1=Albedo, 4=Metallic...)
    //float metallic = (UseTexture & 4) ? t_Metallic.Sample(g_Sampler, uv).r : 0.0f;
    //float roughness = (UseTexture & 8) ? t_Roughness.Sample(g_Sampler, uv).r : 0.5f;
    //float ao = (UseTexture & 16) ? t_AO.Sample(g_Sampler, uv).r : 1.0f;
    
    float metallic =  t_Metallic.Sample(g_Sampler, uv).r;
    float roughness = t_Roughness.Sample(g_Sampler, uv).r;
    float ao =        t_AO.Sample(g_Sampler, uv).r;

    float3 viewDir = normalize(CameraPos - input.WorldPos);

    float3 finalColor = ComputePBRLighting(input.WorldPos, normal, viewDir, albedo, metallic, roughness, ao);

    return float4(finalColor, alpha);
=======
    float3 N = input.WorldNormal;
    float3 L = Lights[0].DirectionType.xyz;
    
    float4 texBase = g_texBase.Sample(g_Sampler, input.UV);     //sRGB8_A8
    float4 texMetal = g_texMetal.Sample(g_Sampler, input.UV).r; //R8
    float4 texRough = g_texRough.Sample(g_Sampler, input.UV).r; //R8
    float4 texNormal = g_texNormal.Sample(g_Sampler, input.UV); //RGBA8
    float4 texAO = g_texAO.Sample(g_Sampler, input.UV).r;       //R8
    //float4 texEnv = g_texEnv.Sample(smpAniso, uvw);           //RGBA
    
    float4 albedo = texBase * texAO;
    
    float4 diff = Lights[0].ColorIntensity.w * max(dot(N, L), 0) / PI * albedo;
    
    
    float4 amb = albedo;

    return amb + diff;
>>>>>>> .merge_file_oJalNe
}