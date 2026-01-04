// pragma once 같은거임

struct LightData
{
    float4 PositionRange;
    float4 DirectionType;
    float4 ColorIntensity;
};


cbuffer CBuffer_Frame : register(b0)
{
    matrix View;
    matrix Projection;
};

cbuffer CBuffer_Object : register(b1)
{
    matrix World;
    matrix WorldInverseTranspose;
};

#define MAX_LIGHTS 4
cbuffer CBuffer_GlobalLight : register(b2)
{
    LightData Lights[MAX_LIGHTS];
    int ActiveCount;
    float3 CameraPos;
    matrix LightViewProjScale;
    float4 ShadowMapInfo; // xy: 1/w 1/h , zw W, H
};

cbuffer CBuffer_Material : register(b3)
{
    float4 MaterialColor;
    float4 UVTransform;
    int UseTexture;
    float2 Padding2;
};



Texture2D g_ShadowMap : register(t10);
SamplerComparisonState g_ShadowSampler : register(s10);

