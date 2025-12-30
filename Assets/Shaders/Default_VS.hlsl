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

cbuffer CBuffer_Material : register(b3)
{
    float4 MaterialColor;   // Material.Color
    float4 UVTransform;     // xy=Tiling, zw=Offset
    int UseTexture;         // Material.UseTexture
    int3 Padding2;
};


struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
};


struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;
};


PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    
    float4 worldPos = mul(float4(input.Pos, 1.0f), WorldTM);
    float4 viewPos  = mul(worldPos, ViewTM);
    float4 projPos  = mul(viewPos, ProjectionTM);
    
    float3 worldNormal = mul(float4(input.Normal, 0.0f), WorldInverseTransposeTM).xyz;

    //output.Pos = float4(input.Pos, 1.0);
    output.Pos = projPos;
    output.UV = input.UV;
    output.WorldPos = worldPos.xyz;
    output.Normal = worldNormal;
    
    
    
    return output;
}

