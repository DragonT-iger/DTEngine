#ifndef _LIGHTING_HLSLI_
#define _LIGHTING_HLSLI_
// pragma once 같은거임

struct LightData
{
    float4 PositionRange; 
    float4 DirectionType; 
    float4 ColorIntensity;
};

#define MAX_LIGHTS 4

cbuffer CBuffer_GlobalLight : register(b2)
{
    LightData Lights[MAX_LIGHTS];
    int ActiveCount;
    float3 CameraPos;
    matrix LightViewProjScale;
};

Texture2D g_ShadowMap : register(t5);
SamplerState g_ShadowSampler : register(s5);

float CalculateShadow(float3 worldPos)
{
    float4 shadowCoord = mul(float4(worldPos, 1.0f), LightViewProjScale);
    
    shadowCoord.xyz /= shadowCoord.w;


    float shadowDepth = g_ShadowMap.Sample(g_ShadowSampler, shadowCoord.xy).r;
    float currentDepth = shadowCoord.z;
    float bias = 0.002f;

    //return shadowDepth;
    return (currentDepth - bias > shadowDepth) ? 0.0f : 1.0f;
}

float3 ComputeLambertLighting(float3 worldPos, float3 normal)
{
    
    float3 totalDiffuse = float3(0, 0, 0);
    
    float3 skyColor = float3(0.35f, 0.35f, 0.35f);
    float3 groundColor = float3(0.1f, 0.1f, 0.1f);
    float up = normal.y * 0.5 + 0.5;
    float3 ambient = lerp(groundColor, skyColor, up);

    float shadowFactor = 1.0f;
    
    shadowFactor = CalculateShadow(worldPos);

    for (int i = 0; i < ActiveCount; ++i)
    {
        
        float3 lightColor = Lights[i].ColorIntensity.rgb;
        float intensity =   Lights[i].ColorIntensity.w;
        float type =        Lights[i].DirectionType.w;

        float3 L; 
        float attenuation;

        // Directional Light
        if (type < 0.5f)
        {
            L = normalize(Lights[i].DirectionType.xyz);
            attenuation = 1.0f;
        }
        // Point Light
        else
        {
            float3 toLight = Lights[i].PositionRange.xyz - worldPos;
            float dist = length(toLight);
            L = normalize(toLight);
            
            float range = Lights[i].PositionRange.w;
            attenuation = saturate(1.0 / (1.0 + 0.1 * dist / range + 0.01 * dist / range * dist / range));
        }

        float NdotL = max(dot(normal, L), 0.0f);

        float currentShadow = (i == 0) ? shadowFactor : 1.0f;

        totalDiffuse += NdotL * lightColor * intensity * attenuation * currentShadow;
    }

    return (ambient + totalDiffuse);
}

#endif