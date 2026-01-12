#include "Resource.hlsli"

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
    float3 WorldPos : POSITION;
    float3 WorldNormal : NORMAL;
    float4 Tangent : TANGENT;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 worldPos = mul(float4(input.Pos, 1.0f), World_TM);
    output.WorldPos = worldPos.xyz;

    output.Pos = mul(worldPos, View_TM);
    output.Pos = mul(output.Pos, Projection_TM);

    // float3 worldNormal      = mul(input.Normal,    (float3x3) WorldInverseTranspose);
    // float3 worldTangent     = mul(input.Tangent,   (float3x3) WorldInverseTranspose);
    // float3 worldBitangent   = mul(input.Bitangent, (float3x3) WorldInverseTranspose);

    
    // output.Normal = normalize(worldNormal);
    // output.Tangent = normalize(worldTangent);
    // output.Bitangent = normalize(worldBitangent);
    
    // //output.Normal       = (worldNormal);
    // //output.Tangent      = (worldTangent);
    // //output.Bitangent    = (worldBitangent);
    
    output.Color = input.Color;
    output.WorldNormal = mul(input.Normal, (float3x3) WorldInverseTranspose_TM);
    output.Tangent = input.Tangent;
    output.UV = input.UV;

    return output;
}