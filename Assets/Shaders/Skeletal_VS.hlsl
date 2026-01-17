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

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;


    float4x4 skinMatrix = (float4x4) 0;
    
    skinMatrix += (float4x4) Matrix_Pallette_Array[input.BoneID.x] * input.Weights.x;
    skinMatrix += (float4x4) Matrix_Pallette_Array[input.BoneID.y] * input.Weights.y;
    skinMatrix += (float4x4) Matrix_Pallette_Array[input.BoneID.z] * input.Weights.z;
    skinMatrix += (float4x4) Matrix_Pallette_Array[input.BoneID.w] * input.Weights.w;

    // 2. 열 우선(Column-Major) 방식의 변환 연산: mul(Vector, Matrix)
    // 로컬 좌표 -> 스킨 변환 -> 월드 변환 [cite: 23]
    float4 modelPos = mul(float4(input.Pos, 1.0f), skinMatrix);
    
    

    
    float4 worldPos = mul(modelPos, World_TM);
    
    

    
    output.WorldPos = worldPos.xyz;

    // View 및 Projection 변환 [cite: 24]
    output.Pos = mul(worldPos, View_TM);
    output.Pos = mul(output.Pos, Projection_TM);

    // 3. 법선 및 탄젠트 변환 (회전 성분만 처리)
    float3x3 skinRotation = (float3x3) skinMatrix;
    float3 skinnormal = mul(input.Normal, skinRotation);
    float3 skintangent = mul(input.Tangent.xyz, skinRotation);
    float3 skinbitangent = mul(input.Bitangent, skinRotation);

    // 월드 공간 법선 변환 (WorldInverseTranspose_TM 사용) [cite: 24, 25]
    output.WorldNormal = normalize(mul(skinnormal, (float3x3) WorldInverseTranspose_TM));
    output.Tangent = float4(normalize(mul(skintangent, (float3x3) WorldInverseTranspose_TM)), input.Tangent.w);
    output.Bitangent = normalize(mul(skinbitangent, (float3x3) WorldInverseTranspose_TM));
    
    output.UV = input.UV;

    return output;
}