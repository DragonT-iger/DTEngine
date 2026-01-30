#include "Resource.hlsli"

SamplerState g_Sampler : register(s0);


struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR;
    float3 WorldNormal : NORMAL;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    // 1. 스키닝 계산 (Skeletal_VS.hlsl의 로직 적용)
    float4x4 skinMatrix = (float4x4) 0;
    
    if (Skinned_Flag >0.5)
    {
    
        skinMatrix += Matrix_Pallette_Array[input.BoneID.x] * input.Weights.x;
        skinMatrix += Matrix_Pallette_Array[input.BoneID.y] * input.Weights.y;
        skinMatrix += Matrix_Pallette_Array[input.BoneID.z] * input.Weights.z;
        skinMatrix += Matrix_Pallette_Array[input.BoneID.w] * input.Weights.w;
    }
    else 
        skinMatrix = float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    
    float4 modelPos = mul(float4(input.Pos, 1.0f), skinMatrix);
    float3 modelNormal = normalize(mul(input.Normal, (float3x3) skinMatrix));


   // float2 noiseUV = frac(input.UV + progress); //
   // //float2 noiseUV = input.UV + (progress * 0.2f) * 100.0f; //
   //// noiseUV = input.UV *100.0f;
    
    float noiseValue = g_NoiseMap.SampleLevel(g_Sampler, input.UV, 0).r;

    float dWidth = noiseValue * edgeWidth;
    
    float3 expandedPos = modelPos.xyz + (modelNormal * edgeWidth);


    
    float4 worldPos = mul(float4(expandedPos, 1.0f), World_TM);
    output.Pos = mul(mul(worldPos, View_TM), Projection_TM);
    
    
    
    output.WorldNormal = normalize(mul(modelNormal, (float3x3) WorldInverseTranspose_TM));
    output.UV = input.UV;
    output.Color = edgeColor;
   
   
    return output;
}
