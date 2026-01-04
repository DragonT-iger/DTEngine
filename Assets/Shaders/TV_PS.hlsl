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
    
    float4 diffuse = Diffuse.Sample(g_Sampler, input.UV);
    float4 rendertarget = RenderTarget.Sample(g_Sampler, input.UV);
    
    rendertarget.rgb += MaterialColor;
    
    float4 finalColor = (1 - diffuse.a) * rendertarget + diffuse.a * diffuse;
    
    clip(finalColor.a - 0.01);
    
    return finalColor;
}