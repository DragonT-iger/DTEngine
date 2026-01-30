
#include "PBR_Shared.hlsli"
SamplerState g_Sampler : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR;
    float3 WorldNormal : NORMAL;
   
};

float4 PS(PS_INPUT input) : SV_Target
{


    
    return input.Color ;
    
  
       
}