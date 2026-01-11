// 기본적인 Lambert Lighting 모델이고 간이 환경광이 적용되어 있음.
// 텍스쳐가 있으면 텍스쳐 색상, 없으면 머티리얼 색상을 사용함.
// Forward 방식임.

#include "Lighting.hlsli"
#include "Resource.hlsli"

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;

};


Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

float4 PS(PS_INPUT input) : SV_Target
{
    float3 normal = normalize(input.Normal);

    float3 finalColor = ComputeLambertLighting(input.WorldPos, normal);
        
    if (UseTexture)
    {
        float2 transformedUV = input.UV * UVTransform.xy + UVTransform.zw;
        
        float4 texColor = g_Texture.Sample(g_Sampler, transformedUV);
        
        
        clip(texColor.a < 0.2 ? -1 : 1);
        
        finalColor *= texColor.rgb;
        
        
        
    }
    else
    {
        finalColor *= MaterialColor.rgb;
    }

    
    if (NEED_ON_GAMMA)
    {
        finalColor = pow(finalColor, 1.0f / 2.2f); // 
    }
    
    return float4(finalColor, 1.0f);
}