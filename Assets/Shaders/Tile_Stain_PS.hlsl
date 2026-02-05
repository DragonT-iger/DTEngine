
#include "Lighting.hlsli"
#include "Resource.hlsli"

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

SamplerState g_Sampler : register(s0);


//Transparent로 처리하거나, Shader에서 처리하거나

float4 PS(PS_INPUT input) : SV_Target
{
    float4 color = g_AoMap.Sample(g_Sampler, input.UV);    
    float mask = g_NoiseMap.Sample(g_Sampler, input.UV).r;
        
    clip( 0.01f - MaterialColor.a);

   // float alpha = MaterialColor.a;
    
   

    return float4(color.rgb, mask);
    
   
}