#include "Resource.hlsli"

//struct VS_INPUT
//{
//    float3 Pos : POSITION;
//    //float4 Color : COLOR;
//    float2 UV : TEXCOORD;
//    float3 Normal : NORMAL;
//    float4 Tangent : TANGENT;
//   // float3 Bitangent : BITANGENT;
//};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    //float4 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 worldPos = mul(float4(input.Pos, 1.0f), World_TM);
    output.WorldPos = worldPos.xyz;

    output.Pos = mul(worldPos, View_TM);
    output.Pos = mul(output.Pos, Projection_TM);

    float3 worldNormal      = mul(input.Normal,    (float3x3) WorldInverseTranspose_TM);
    float3 worldTangent     = mul(input.Tangent.xyz,   (float3x3) WorldInverseTranspose_TM);
    float3 worldBitangent   = mul(input.Bitangent, (float3x3) WorldInverseTranspose_TM);

    
    //output.Normal = normalize(worldNormal);
    //output.Tangent = normalize(worldTangent);
    //output.Bitangent = normalize(worldBitangent);
    
    output.Normal       = worldNormal;
    output.Tangent = float4(normalize(worldTangent), input.Tangent.w);
    output.Bitangent    = (worldBitangent);
    
    //output.Color = input.Color;
    output.UV = input.UV;

    return output;
}