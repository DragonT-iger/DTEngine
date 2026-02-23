
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
        float2 transformedUV = input.UV * UVTransform.xy + UVTransform.zw;
        
        float4 texColor = g_Texture.Sample(g_Sampler, transformedUV);
        
        
        clip(texColor.a < 0.2 ? -1 : 1);
        
    return float4(texColor.rgb, 1.0f);
}