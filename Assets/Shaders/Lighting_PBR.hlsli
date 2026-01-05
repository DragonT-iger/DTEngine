#ifndef _LIGHTING_PBR_HLSLI_
#define _LIGHTING_PBR_HLSLI_

#include "Lighting.hlsli"

static const float PI = 3.14159265359f;

// 1. 노멀 분포 함수 (GGX)
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001);
}

// 2. 기하 함수 (Smith-Schlick)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// 3. 프레넬 방정식
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float3 ComputePBRLighting(float3 worldPos, float3 normal, float3 viewDir,
                          float3 albedo, float metallic, float roughness, float ao)
{
    float3 N = normalize(normal);
    float3 V = normalize(viewDir);
    
    float3 F0 = 0.03;
    F0 = lerp(F0, albedo, metallic);

    float3 Lo = 0;

    float shadowFactor = CalculateShadow(worldPos);

    for (int i = 0; i < ActiveCount; ++i)
    {
        float3 lightPos = Lights[i].PositionRange.xyz;
        float3 lightColor = Lights[i].ColorIntensity.rgb * Lights[i].ColorIntensity.w;
        float lightType = Lights[i].DirectionType.w;

        float3 L;
        float attenuation = 1.0;

        // Directional Light
        if (lightType < 0.5f)
        {
            L = normalize(Lights[i].DirectionType.xyz);
        }
        // Point Light
        else
        {
            float3 toLight = lightPos - worldPos;
            float distance = length(toLight);
            L = normalize(toLight);
            float range = Lights[i].PositionRange.w;
            
            attenuation = saturate(1.0 / (1.0 + 0.1 * distance / range + 0.01 * distance / range * distance / range));
        }

        float3 H = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        
        float3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * NdotL + 0.001;
        float3 specular = nominator / denominator;
        
        float3 kS = F;
        float3 kD = 1 - kS;
        kD *= (1.0 - metallic);

        float currentShadow = (i == 0) ? shadowFactor : 1.0f;

        //Lo += (kD * albedo / PI  + specular) * lightColor * NdotL * attenuation * currentShadow; // PI 나누기 버전 (어두움)
        Lo += (kD * albedo + specular) * lightColor * NdotL * attenuation * currentShadow;
    }

    float3 ambient = 0.03f * albedo * ao;
    float3 color = ambient + Lo;

    
    return color;
}
#endif