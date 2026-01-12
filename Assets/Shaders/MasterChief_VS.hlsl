cbuffer CBuffer_Frame : register(b0)
{
    matrix View;
    matrix Projection;
};

cbuffer CBuffer_Object : register(b1)
{
    matrix World;
    matrix WorldInverseTranspose;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    //float4 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
   // float3 Bitangent : BITANGENT;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    //float4 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    //float3 Bitangent : BITANGENT;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 worldPos = mul(float4(input.Pos, 1.0f), World);
    output.WorldPos = worldPos.xyz;

    output.Pos = mul(worldPos, View);
    output.Pos = mul(output.Pos, Projection);

    //float3 worldNormal      = mul(input.Normal,    (float3x3) WorldInverseTranspose);
    //float3 worldTangent     = mul(input.Tangent,   (float3x3) WorldInverseTranspose);
    //float3 worldBitangent   = mul(input.Bitangent, (float3x3) WorldInverseTranspose);

    
    //output.Normal = normalize(worldNormal);
    //output.Tangent = normalize(worldTangent);
    //output.Bitangent = normalize(worldBitangent);
    
    //output.Normal       = (worldNormal);
    //output.Tangent      = (worldTangent);
    //output.Bitangent    = (worldBitangent);
    
    //output.Color = input.Color;
    output.UV = input.UV;

    return output;
}