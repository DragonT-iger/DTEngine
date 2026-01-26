Texture2D InputTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer BloomCBuffer : register(b0)
{
    float threshold;
    float intensity;
    float2 blurDir; 
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

static const float weight[3] = { 0.4026, 0.2442, 0.0545 };

float4 PS(PS_INPUT input) : SV_TARGET
{
    float width, height;
    InputTexture.GetDimensions(width, height);
    float2 texelSize = 1.0f / float2(width, height);

    // 현재 픽셀 (중심)
    float4 result = InputTexture.Sample(Sampler, input.uv) * weight[0];

    for (int i = 1; i < 3; ++i)
    {
        float2 offset = blurDir * (texelSize * float(i));
        
        result += InputTexture.Sample(Sampler, input.uv + offset) * weight[i];
        result += InputTexture.Sample(Sampler, input.uv - offset) * weight[i];
    }

    return result;
}