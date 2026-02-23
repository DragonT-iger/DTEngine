// [Assets/Shaders/ToneMapping_PS.hlsl]

Texture2D SceneTexture : register(t0);
SamplerState DefaultSampler : register(s0);

cbuffer ToneMappingBuffer : register(b0)
{
    float Exposure; 
    float3 Padding; 
};

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 hdrColor = SceneTexture.Sample(DefaultSampler, input.TexCoord);
    float3 color = hdrColor.rgb;

   
    color *= Exposure;

  
    color = ACESFilm(color);
 
    return float4(color, 1.0f);
}