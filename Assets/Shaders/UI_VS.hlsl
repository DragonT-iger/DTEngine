#include "Resource.hlsli"

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 worldPos = mul(float4(input.Pos, 1.0f), World_TM);
    float4 viewPos = mul(worldPos, View_TM);
    float4 projPos = mul(viewPos, Projection_TM);
    
    output.Pos = projPos;
    output.UV = input.UV;
    
    return output;
}