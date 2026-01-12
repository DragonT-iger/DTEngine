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

    
    //matrix boneTransform =
    //    input.Weights.x * BoneTransforms[input.BoneIDs.x] +
    //    input.Weights.y * BoneTransforms[input.BoneIDs.y] +
    //    input.Weights.z * BoneTransforms[input.BoneIDs.z] +
    //    input.Weights.w * BoneTransforms[input.BoneIDs.w];
    
    matrix boneTransform =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    
    
    float4 localPos = float4(input.Pos, 1.0f);
    float4 animatedLocalPos = mul(localPos, boneTransform);
    float4 worldPos = mul(animatedLocalPos, WorldTM);
    float4 viewPos  = mul(worldPos, ViewTM);
    float4 projPos  = mul(viewPos, ProjectionTM);
    
    float4 localNormal = float4(input.Normal, 0.0f);
    float4 animatedLocalNormal = mul(localNormal, boneTransform);
    float3 worldNormal = mul(animatedLocalNormal, WorldInverseTransposeTM).xyz;

    //output.Pos = float4(input.Pos, 1.0);
    output.Pos = projPos;
    output.UV = input.UV;
    output.WorldPos = worldPos.xyz;
    output.Normal = worldNormal;
    output.Tangent = input.Tangent;
    
    
    return output;
}

