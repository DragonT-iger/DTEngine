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
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 TexCoord : TEXCOORD0; 
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 worldPos = mul(float4(input.Pos, 1.0f), World);
    output.Pos = mul(worldPos, View);
    output.Pos = mul(output.Pos, Projection);

    output.TexCoord = input.Pos;
    
    return output;
}