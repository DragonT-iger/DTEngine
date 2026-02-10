#include "Resource.hlsli"

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

Texture2D Diffuse : register(t0);
Texture2D RenderTarget : register(t1);
SamplerState g_Sampler : register(s0);

float4 PS(PS_INPUT input) : SV_Target
{    
    float4 color = MaterialColor;
    
    float4 textureColor = 1;
    
    if (UseTexture)
    {
        textureColor = g_DiffuseMap.SampleLevel(g_Sampler, input.UV, 0);
    }

    color *= textureColor;
    
    return color;
}