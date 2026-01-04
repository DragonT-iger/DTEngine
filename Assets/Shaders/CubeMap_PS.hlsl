struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;
};

TextureCube g_CubeMap : register(t0);
SamplerState g_Sampler : register(s0);

#include "Lighting.hlsli"

cbuffer CBuffer_Material : register(b3)
{
    float4 MaterialColor;
    float4 UVTransform;
    int     UseTexture;
    float2 Padding2;
};

float4 PS(PS_INPUT input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float3 viewDir = normalize(input.WorldPos - CameraPos); 

    float3 reflectDir = reflect(viewDir, normal);

    float4 envColor = g_CubeMap.Sample(g_Sampler, reflectDir);

    return envColor * CalculateShadow(input.WorldPos);
}