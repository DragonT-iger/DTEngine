struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;
};



Texture2D g_texBase : register(t0);
Texture2D g_texMetal : register(t1);
Texture2D g_texRough : register(t2);
Texture2D g_texNormal : register(t3);
Texture2D g_texAO : register(t4);
TextureCube g_texEnv : register(t5);

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
    
    
    float4 texBase = g_texBase.Sample(g_Sampler, input.UV);     //sRGB8_A8
    float4 texMetal = g_texMetal.Sample(g_Sampler, input.UV).r; //R8
    float4 texRough = g_texRough.Sample(g_Sampler, input.UV).r; //R8
    float4 texNormal = g_texNormal.Sample(g_Sampler, input.UV); //RGBA8
    float4 texAO = g_texAO.Sample(g_Sampler, input.UV).r;       //R8
    //float4 texEnv = g_texEnv.Sample(smpAniso, uvw);             //RGBA
    

    return texBase;
}