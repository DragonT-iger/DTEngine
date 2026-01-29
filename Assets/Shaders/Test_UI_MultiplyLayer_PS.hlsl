
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

Texture2D g_UiTex : register(t0);
Texture2D g_ColorTex : register(t1);

SamplerState g_Sampler : register(s0);


//Transparent로 처리하거나, Shader에서 처리하거나

float4 PS(PS_INPUT input) : SV_Target
{
    float4 U = g_UiTex.Sample(g_Sampler, input.UV);
    float4 C = g_ColorTex.Sample(g_Sampler, input.UV);
    
    float a = Shadow_Scale;


    float3 rgb = lerp(U.rgb, U.rgb * C.rgb, a);

    return float4(rgb, U.a);
}