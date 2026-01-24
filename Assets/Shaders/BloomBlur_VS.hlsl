struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

VS_OUTPUT VS(uint id : SV_VertexID)
{
    VS_OUTPUT output;
    
    float2 tex = float2((id << 1) & 2, id & 2);
    output.UV = tex;
    output.Pos = float4(tex * float2(2, -2) + float2(-1, 1), 0, 1);
    
    return output;
}
