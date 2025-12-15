cbuffer CBuffer_Frame : register(b0)
{
    matrix ViewTM;
    matrix ProjectionTM;
};

cbuffer CBuffer_Object : register(b1)
{
    matrix WorldTM;
    matrix WorldInverseTransposeTM;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
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

    float4 worldPos = mul(float4(input.Pos, 1.0f), WorldTM);
    float4 viewPos = mul(worldPos, ViewTM);
    float4 projPos = mul(viewPos, ProjectionTM);
    
    float3 worldNormal = mul(float4(input.Normal, 0.0f), WorldInverseTransposeTM).xyz;
    float3 worldTangent = mul(float4(input.Tangent, 0.0f), WorldInverseTransposeTM).xyz;
    float3 worldBitangent = mul(float4(input.Bitangent, 0.0f), WorldInverseTransposeTM).xyz;

    float3 viewNormal = mul(worldNormal, (float3x3) ViewTM);

    output.Pos = projPos;
    output.Color = input.Color;
    output.UV = input.UV;
    output.WorldPos = worldPos.xyz;
    output.Normal = worldNormal;
    output.Tangent = worldTangent;
    output.Bitangent = worldBitangent;
    output.ViewNormal = viewNormal;
    
    return output;
}