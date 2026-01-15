#ifndef _LIGHTING_HLSLI_
#define _LIGHTING_HLSLI_
// pragma once 같은거임

struct LightData
{
    float4 PositionRange;
    float4 DirectionType;
    float4 ColorIntensity;
};

#define PCF_LOOP 1
#define PCF_DEVISOR 9
//(PCF_LOOP * 2 + 1) / PCF_LOOP 최적화를 위해

#define MAX_LIGHTS 4
cbuffer CBuffer_GlobalLight : register(b3)
{
    LightData Lights[MAX_LIGHTS];
    int ActiveCount;
    float3 CameraPos;
    
    matrix LightViewProjScale;
    float4 ShadowMapInfo; // xy: 1/w 1/h , zw W, H
};

Texture2D g_ShadowMap : register(t10);
SamplerComparisonState g_ShadowSampler : register(s10);

float CalculateShadow(float3 worldPos)
{
    float4 shadowCoord = mul(float4(worldPos, 1.0f), LightViewProjScale);
    
    shadowCoord.xyz /= shadowCoord.w;

    // 화면(NDC) 밖이면 그림자 없음 처리
    // 아래 코드보다 이게 부하 더 적음
    //if (shadowCoord.x < 0.0f || shadowCoord.x > 1.0f ||
    //    shadowCoord.y < 0.0f || shadowCoord.y > 1.0f ||
    //    shadowCoord.z < 0.0f || shadowCoord.z > 1.0f)
    //{
    //    return 1.0f;
    //}
   
    
    float currentDepth = shadowCoord.z;
    float bias = 0.005f;
    
    float totalShadow = 0;
    
    //for (int i = -PCF_LOOP; i <= PCF_LOOP; i++)
    //{
    //    for (int j = -PCF_LOOP; j <= PCF_LOOP; j++)
    //    {
    //        float2 offset = ShadowMapInfo.xy * float2(i, j);
            
    //        //float shadowDepth = g_ShadowMap.Sample(g_ShadowSampler, shadowCoord.xy + offset).r;
    //        //totalShadow += (currentDepth - bias > shadowDepth) ? 0.0f : 1.0f;
            
    totalShadow = g_ShadowMap.SampleCmpLevelZero(g_ShadowSampler, shadowCoord.xy, currentDepth - bias);
    //    }
    //}
    
    return totalShadow;
    
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