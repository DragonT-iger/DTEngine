Texture2D SceneTexture : register(t0);
Texture2D BloomTexture : register(t1);
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

float4 PS(PS_INPUT input) : SV_TARGET
{
    float4 sceneColor = SceneTexture.Sample(Sampler, input.uv);
    float4 bloomColor = BloomTexture.Sample(Sampler, input.uv);

    return float4(sceneColor.rgb + bloomColor.rgb * intensity, sceneColor.a);
}