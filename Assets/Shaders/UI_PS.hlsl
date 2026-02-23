#include "Resource.hlsli"

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};



SamplerState g_Sampler : register(s0);

float4 PS(PS_INPUT input) : SV_Target
{
    float4 color = MaterialColor;
    
    //clip(color.a < 0.01f ? -1 : 1);
    //color.rgb = pow(color.rgb, 2.2f); SRGB 사용
    
    float4 textureColor = 1;
    
    if (UseTexture) 
    {
        textureColor = g_DiffuseMap.SampleLevel(g_Sampler, input.UV, 0);
    }

    //clip(textureColor.r == 1.0f && textureColor.g == 0.0f ? -1 : 1);
    // 좀 짜친다..
    
    //clip(textureColor.g < 0.009f ? -1 : 1);
    
    
    color *= textureColor;
    
    
    
    return color;
}