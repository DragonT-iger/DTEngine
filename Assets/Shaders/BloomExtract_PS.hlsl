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

float4 PS(PS_INPUT input) : SV_TARGET
{
    float4 color = InputTexture.Sample(Sampler, input.uv);

    // 픽셀의 밝기(Luminance) 계산 (사람 눈에 민감한 녹색에 가중치)
    float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));

    if (brightness > threshold)
    {
        // 밝기가 임계값을 넘으면 원본 색상 반환
        return color;
    }
    else
    {
        // 아니면 검은색 반환
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}