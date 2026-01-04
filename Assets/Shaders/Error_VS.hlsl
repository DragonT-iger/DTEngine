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
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
};


struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};


PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    
    float4 worldPos = mul(float4(input.Pos, 1.0f), WorldTM);
    float4 viewPos = mul(worldPos, ViewTM);
    float4 projPos = mul(viewPos, ProjectionTM);
    
    //output.Pos = float4(input.Pos, 1.0);
    output.Pos = projPos;
    
    
    
    return output;
}

