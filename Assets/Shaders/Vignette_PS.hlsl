Texture2D InputTexture : register(t0);
SamplerState InputSampler : register(s0);


cbuffer VignetteParams : register(b0)
{
    float Radius;   
    float Softness; 
    float2 Padding;
}

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
    
    float vignette = 1.0f - smoothstep(Softness, Radius, len);
    
    return color * vignette;
}