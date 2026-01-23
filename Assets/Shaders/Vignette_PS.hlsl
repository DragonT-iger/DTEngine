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
    
    float2 distFromCenter = input.UV - 0.5f;
    
    float len = length(distFromCenter);
    
    float vignette = 1.0f - smoothstep(0.4f, 0.8f, len);
    
    return color * vignette;
}