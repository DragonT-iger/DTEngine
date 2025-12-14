// Assets/Shaders/Reflection_PS.hlsl

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
    float Padding;
};

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

    return envColor;
}