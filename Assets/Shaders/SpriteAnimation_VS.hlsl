#include "Resource.hlsli"

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    
        VS_OUTPUT output;

        float3 worldPosition = float3(World_TM._41, World_TM._42, World_TM._43);

        float3x3 w3 = (float3x3) World_TM;
        float3 worldAxisX = w3[0];
        float3 worldAxisY = w3[1];
        float3 worldAxisZ = w3[2];

        float3 scale;
        scale.x = length(worldAxisX);
        scale.y = length(worldAxisY);
        scale.z = length(worldAxisZ);

 
        float3 localScaled = input.Pos * scale;

        float3 billpos = mul(localScaled, (float3x3) invcamerarotation);

        float4 worldPos = float4(billpos + worldPosition, 1.0f);

        float4 viewPos = mul(worldPos, View_TM);
        output.Pos = mul(viewPos, Projection_TM);

        output.UV = input.UV * UVTransform.xy + UVTransform.zw;
        return output;
    }

