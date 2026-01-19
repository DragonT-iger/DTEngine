#include "Lighting_PBR.hlsli"
#include "Resource.hlsli"

//cbuffer CBuffer_Frame : register(b0)
//{
//    matrix ViewTM;
//    matrix ProjectionTM;
//};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    //float4 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

//cbuffer CBuffer_Material : register(b3)
//{
//    float4 MaterialColor;
//    float4 UVTransform;
//    int UseTexture;
//    int3 Padding2;
//};

Texture2D t_Albedo : register(t0);
Texture2D t_MetallicRoughness : register(t1);
Texture2D t_Normal : register(t2);
Texture2D t_sphereMap : register(t3);

SamplerState g_Sampler : register(s0);


float3 CalculateNormal(PS_INPUT input, float2 uv)
{
    float3 normalSample = t_Normal.Sample(g_Sampler, uv).rgb;
    float3 tangentNormal = normalSample * 2.0 - 1.0;

    float3 N = normalize(input.Normal);
    float3 T = normalize(input.Tangent.xyz);
    float3 B = normalize(input.Bitangent);

    float3x3 TBN = float3x3(T, B, N);

    return normalize(mul(tangentNormal, TBN));
}


//float3 GetWorldNormalFromNormalMap(float4 texNormal, float3 N, float4 tangent)
//{
//    float3 nTS = texNormal.xyz * 2.0f - 1.0f;

//    float3 T = tangent.xyz;
    
//    T = normalize(T - dot(T, N) * N);
//    float3 B = normalize(cross(N, T)) * tangent.w;

//    float3x3 TBN = float3x3(T, B, N);
//    return normalize(mul(nTS, TBN));
//}


float4 PS(PS_INPUT input) : SV_Target
{
    float4 albedoSample = t_Albedo.Sample(g_Sampler, input.UV);
    float3 albedo = albedoSample.rgb;
    float alpha = albedoSample.a;

    // Alpha Clipping
    //clip(alpha < 0.1f ? -1 : 1);

   
    float3 normal = CalculateNormal(input, input.UV);
    
    
    
    
    //float3 normal = input.Normal;
    
    // 노멀맵 이따가 적용

    // UseTexture 비트 플래그를 활용해 제어한다고 가정 (예: 1=Albedo, 4=Metallic...)
    //float metallic = (UseTexture & 4) ? t_Metallic.Sample(g_Sampler, uv).r : 0.0f;
    //float roughness = (UseTexture & 8) ? t_Roughness.Sample(g_Sampler, uv).r : 0.5f;
    //float ao = (UseTexture & 16) ? t_AO.Sample(g_Sampler, uv).r : 1.0f;
    
    float metallic = t_MetallicRoughness.Sample(g_Sampler, input.UV).b;
    float roughness = t_MetallicRoughness.Sample(g_Sampler, input.UV).g;
    
    //float metallic = 1;
    //float roughness = 1;
    
    //float ao = t_AO.Sample(g_Sampler, uv).r;

    float3 viewDir;
    
    if (IsOrtho > 0.5)
    {
        viewDir = -normalize(CameraDir);
    }
    else
    {
        viewDir = normalize(CameraPos - input.WorldPos);
    }
    

    float3 viewNormal = normalize(mul(normal, (float3x3) View_TM));
    float2 sphereUV = viewNormal.xy * 0.5 + 0.5;
    sphereUV.y = 1.0 - sphereUV.y;
    
    float3 sphereEnvLight = t_sphereMap.Sample(g_Sampler, sphereUV).rgb;
    
    if (metallic < 0.99)
    {
        sphereEnvLight = 0;
    }
    
    float3 finalColor = ComputePBRLighting(input.WorldPos, normal, viewDir, albedo, metallic, roughness, 1, sphereEnvLight);
    
    
    //return finalColor;
    return float4(finalColor, 1.0f);
}