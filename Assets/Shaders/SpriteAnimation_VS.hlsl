#include "Resource.hlsli"

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    

    float3 billpos = mul(input.Pos, (float3x3) invcamerarotation);
    float3 worldPosition = float3(World_TM._41, World_TM._42, World_TM._43);
    float4 worldPos = float4(billpos + worldPosition, 1.0f);
    float4 viewPos = mul(worldPos, View_TM);
    output.Pos = mul(viewPos, Projection_TM);
    
    output.UV = input.UV * UVTransform.xy + UVTransform.zw;

    return output;
}