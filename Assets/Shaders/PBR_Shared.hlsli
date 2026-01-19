#ifndef _PBR_SHARED_HLSLI_
#define _PBR_SHARED_HLSLI_

#include "Resource.hlsli"
#include "Lighting.hlsli"

#define PI 3.1415926535



// Sphere_2.fx 쪽 구현 스타일과 동일 계열
float SchlickFresnel_Disney(float u)
{
    float m = saturate(1.0f - u);
    float m2 = m * m;
    return m2 * m2 * m; // pow(m, 5)
}

float GTR2(float NdotH, float a)
{
    float a2 = a * a;
    float t = 1.0f + (a2 - 1.0f) * NdotH * NdotH;
    return a2 / (PI * t * t);
}

float smithG_GGX(float NdotV, float alphaG)
{
    float a = alphaG * alphaG;
    float b = NdotV * NdotV;
    return 1.0f / (NdotV + sqrt(a + b - a * b));
}

// Tangent-space normal map -> World normal
float3 GetWorldNormalFromNormalMap(float4 texNormal, float3 N, float4 tangent , float3 Bitangent)
{
    float3 nTS = texNormal.xyz * 2.0f - 1.0f;

    float3 T = tangent.xyz;
    float3 B = Bitangent;
    
    //T = normalize(T - dot(T, N) * N);
    //float3 B = normalize(cross(N, T)) * tangent.w;

    float3x3 TBN = float3x3(T, B, N);
    return normalize(mul(nTS, TBN));
}

// "Sphere_2.fx 스타일(Disney 계열)" PBR
float3 DisneyPBR(
    float3 worldPos,
    float3 N, // world normal (normalized)
    float3 V,
    float3 albedo,
    float roughness, // [0..1]
    float metallic, // [0..1]
    float3 L, // light dir (world, normalized)
    float3 lightRGB, // light color
    float intensity // light intensity
)
{
    //float3 V = normalize(CameraPos - worldPos); // Lighting.hlsli cbuffer의 CameraPos 사용(:contentReference[oaicite:6]{index=6}) Ortho 호환 X
    float3 H = normalize(L + V);

    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));
    float NdotH = saturate(dot(N, H));
    float LdotH = saturate(dot(L, H));

    float FL = SchlickFresnel_Disney(NdotL);
    float FV = SchlickFresnel_Disney(NdotV);
    float Fd90 = 0.5f + 2.0f * LdotH * LdotH * roughness;
    float Fd = lerp(1.0f, Fd90, FL) * lerp(1.0f, Fd90, FV);
    float3 diffusePart = (albedo / PI) * Fd;

    float alpha = max(roughness * roughness, 1e-4f);
    float D = GTR2(NdotH, alpha);

    float3 R0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);
    float3 F = R0 + (1.0f - R0) * SchlickFresnel_Disney(LdotH);
    float G = smithG_GGX(NdotL, alpha) * smithG_GGX(NdotV, alpha);

    // (Sphere_2.fx처럼) 스펙 항을 D*F*G 형태로 두고, 여기서 필요하면 / (4*NdotL*NdotV) 형태로 보정 가능
    float3 specularPart = (D * F * G);

    float3 brdf = (diffusePart * (1.0f - metallic) + specularPart);
    return brdf * lightRGB * intensity * NdotL;
}


float3 CalculateIBL_Combined(
    float3 envColor,
    float3 ambientDiffuseColor,
    float3 V, float3 N, float3 albedo,
    float rough, float metal, float ao)
{
   // 1. F0 (수직 입사 반사율) 계산 

    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metal);
    
   // 2. Fresnel (Schlick) 근사 

    float NdotV = saturate(dot(N, V));
    
    float3 F = F0 + (1.0f - F0) * pow(1.0f - NdotV, 5.0f);
    // F = F0 + (max(float3(1.0f - rough, 1.0f - rough, 1.0f - rough), F0) - F0) * pow(1.0f - NdotV, 5.0f);
    // 3. 에너지 보존 (kS: Specular 기여도, kD: Diffuse 기여도) 

    float3 kS = F;
    float3 kD = (1.0f - kS) * (1.0f - metal);// 금속은 Diffuse가 없음

    // 4. 최종 Ambient 합성 

    float3 diffuseIBL = kD * ambientDiffuseColor * albedo;
    float3 specularIBL = envColor * F;

   // 5. AO 적용 및 반환 
    return (diffuseIBL + specularIBL);
}


float3 ACESToneMapping(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

#endif 