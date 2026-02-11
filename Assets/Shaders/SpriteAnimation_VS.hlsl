#include "Resource.hlsli"

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    
    VS_OUTPUT output;

    // 1. 월드 행렬에서 위치와 스케일 추출
    float3 worldPosition = float3(World_TM._41, World_TM._42, World_TM._43);

    float3x3 w3 = (float3x3) World_TM;
    float3 scale;
    scale.x = length(w3[0]);
    scale.y = length(w3[1]);
    scale.z = length(w3[2]);

    float3 localPos = input.Pos * scale;

    float3 rotatedPos = mul(localPos, (float3x3) ownRotationMatrix);

  
    float3 billpos = mul(rotatedPos, (float3x3) invcamerarotation);

    float4 worldPos = float4(billpos + worldPosition, 1.0f);

    float4 viewPos = mul(worldPos, View_TM);
    output.Pos = mul(viewPos, Projection_TM);

    output.UV = input.UV * UVTransform.xy + UVTransform.zw;
    
    return output;
}

