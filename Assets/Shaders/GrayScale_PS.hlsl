Texture2D InputTexture : register(t0);
SamplerState InputSampler : register(s0);

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 color = InputTexture.Sample(InputSampler, input.UV);
    
    float gray = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));
    
    return float4(gray, gray, gray, color.a);
}