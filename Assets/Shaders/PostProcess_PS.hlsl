Texture2D mainTex : register(t0);
SamplerState samLinear : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target
{
    return mainTex.Sample(samLinear, input.UV);
}