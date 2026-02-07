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

    // 2. 노출(Exposure) 조정
    // - Exposure가 높으면 전체적으로 밝아지고, 낮으면 어두워집니다.
    // - 너무 어두운 씬이라면 여기서 Exposure를 2.0 정도로 올리세요.
    color *= Exposure;

    // 3. 톤매핑 적용 (HDR -> LDR 변환)
    // - 1.0을 넘는 밝은 값들을 0~1 사이로 예쁘게 압축합니다.
    color = ACESFilm(color);
   // color = pow(color.rgb, 1.0f / 2.2f);
    //Gamma 는 기존 blit 에서 처리 .
    
    //const float gamma = 2.2f;
    //color = pow(color, 1.0f / gamma);
    return float4(color, 1.0f);
}