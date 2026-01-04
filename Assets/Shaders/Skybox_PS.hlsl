TextureCube g_CubeMap : register(t0); //넌 거기서 살아! 
SamplerState g_Sampler : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 TexCoord : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target
{
    return g_CubeMap.Sample(g_Sampler, input.TexCoord);
}