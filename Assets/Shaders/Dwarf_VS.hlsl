#include "Resource.hlsli"

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;         // 월드 공간 법선 (라이팅 계산용)
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float3 ViewNormal : TEXCOORD1;  // 뷰 공간 법선 (Sphere Map용)
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    float4 worldPos = mul(float4(input.Pos, 1.0f), World_TM);
    float4 viewPos = mul(worldPos, View_TM);
    float4 projPos = mul(viewPos, Projection_TM);
    
    float3 worldNormal = mul(float4(input.Normal, 0.0f), WorldInverseTranspose_TM).xyz;
    float3 worldTangent = mul((input.Tangent), WorldInverseTranspose_TM).xyz;
    float3 worldBitangent = mul(input.Bitangent, (float3x3) WorldInverseTranspose_TM);

    float3 viewNormal = mul(worldNormal, (float3x3) View_TM);

    output.Pos = projPos;
    output.UV = input.UV;
    output.WorldPos = worldPos.xyz;
    output.Normal = worldNormal;
    output.Tangent = worldTangent;
    output.Bitangent = worldBitangent;
    output.ViewNormal = viewNormal;
    
    return output;
}