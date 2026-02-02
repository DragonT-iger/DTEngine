#include "Resource.hlsli"

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
    float3 WorldPos : POSITION;
    float3 WorldNormal : NORMAL;
    float4 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

VS_OUTPUT VS (VS_INPUT input)
{
    
    VS_OUTPUT output;
    
    float4x4 skinMatrix = float4x4(
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
);
    
    skinMatrix += Matrix_Pallette_Array[input.BoneID.x] * input.Weights.x;
    skinMatrix += Matrix_Pallette_Array[input.BoneID.y] * input.Weights.y;
    skinMatrix += Matrix_Pallette_Array[input.BoneID.z] * input.Weights.z;
    skinMatrix += Matrix_Pallette_Array[input.BoneID.w] * input.Weights.w;

   
    float4 modelPos = mul(float4(input.Pos, 1.0f), skinMatrix);
    
    float4 worldPos = mul(modelPos, World_TM);
    
  
    
    output.WorldPos = worldPos.xyz;

    output.Pos = mul(worldPos, View_TM);
    output.Pos = mul(output.Pos, Projection_TM);

    float3x3 skinRotation = (float3x3) skinMatrix;
    float3 skinnormal = mul(input.Normal, skinRotation);
    float3 skintangent = mul(input.Tangent.xyz, skinRotation);
    float3 skinbitangent = mul(input.Bitangent, skinRotation);

    output.WorldNormal = normalize(mul(skinnormal, (float3x3) WorldInverseTranspose_TM));
    output.Tangent = float4(normalize(mul(skintangent, (float3x3) WorldInverseTranspose_TM)), input.Tangent.w);
    output.Bitangent = normalize(mul(skinbitangent, (float3x3) WorldInverseTranspose_TM));
    
    output.UV = input.UV;
    
    return output;
}