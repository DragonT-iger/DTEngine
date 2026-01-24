#include "Resource.hlsli"

struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float2 UV : TEXCOORD0;
    float3 WorldPos : POSITION;
    float3 WorldNormal : NORMAL;
    float4 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 worldPos;
    float3x3 worldRot;

    if (Skinned_Flag > 0.5f)
    {
        float4x4 skinMatrix = Matrix_Pallette_Array[input.BoneID.x] * input.Weights.x;
        skinMatrix += Matrix_Pallette_Array[input.BoneID.y] * input.Weights.y;
        skinMatrix += Matrix_Pallette_Array[input.BoneID.z] * input.Weights.z;
        skinMatrix += Matrix_Pallette_Array[input.BoneID.w] * input.Weights.w;

        float4 modelPos = mul(float4(input.Pos, 1.0f), skinMatrix);
        worldPos = mul(modelPos, World_TM);
        worldRot = mul((float3x3) skinMatrix, (float3x3) WorldInverseTranspose_TM);
    }
    else
    {
        worldPos = mul(float4(input.Pos, 1.0f), World_TM);
        worldRot = (float3x3) WorldInverseTranspose_TM;
    }

    // 2. 공통 변환 및 데이터 전달
    output.WorldPos = worldPos.xyz;
    output.PosH = mul(mul(worldPos, View_TM), Projection_TM);
    output.UV = input.UV;
    
    output.WorldNormal = normalize(mul(input.Normal, worldRot));
    output.Tangent = float4(normalize(mul(input.Tangent.xyz, worldRot)), input.Tangent.w);
    output.Bitangent = normalize(mul(input.Bitangent, worldRot));

    return output;
}