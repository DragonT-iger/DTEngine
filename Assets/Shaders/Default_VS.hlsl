#include "Resource.hlsli"

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;

};


PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    
    float4 worldPos = mul(float4(input.Pos, 1.0f), World_TM);
    float4 viewPos = mul(worldPos, View_TM);
    float4 projPos = mul(viewPos, Projection_TM);
    
    float3 worldNormal = mul(float4(input.Normal, 0.0f), WorldInverseTranspose_TM).xyz;

    //output.Pos = float4(input.Pos, 1.0);
    output.Pos = projPos;
    output.UV = input.UV;
    output.WorldPos = worldPos.xyz;
    output.Normal = worldNormal;
    output.Tangent = input.Tangent;
    
    
    return output;
}

