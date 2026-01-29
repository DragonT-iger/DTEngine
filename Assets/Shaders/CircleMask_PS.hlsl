Texture2D InputTexture : register(t0);
SamplerState InputSampler : register(s0);


cbuffer VignetteParams : register(b0)
{
    float P_Width;  // 렌더타겟  해상도
    float P_Height; // 렌더타겟 해상도
    float Time;  //보류
    float pad;
    
     
    float2 Center; // 원의 중심 (UV 좌표계)
    float2 Width_Height;
    
}

cbuffer CBuffer_Effect : register(b8)
{
    float progress; //진행도 
    float edgeWidth;
    float glowIntensity; //빛의 강도 
    float totalTime; //
    
    float4 edgeColor;
    
    float noiseScalef;
    float timeMultiplier; // 애니메이션 배속 (추가)
    float effectType;
    float Skinned_Flag; // bone / rigid  object와의 통합 위해.
}


struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 color = InputTexture.Sample(InputSampler, input.UV);
    float aspectRatio = P_Width / P_Height;
    float2 d = input.UV - Center;
    d.x *= aspectRatio;

    float2 ellipticalDist = d / Width_Height;
    float len = length(ellipticalDist);

    // --- 튕기는(Bounce) 로직 추가 ---
    float t = saturate(progress);
    
    // Back Out 공식: 목표 지점을 살짝 넘겼다가 돌아옴
    // c1과 c3 값을 조절하여 튕기는 강도를 조절할 수 있습니다.
   // const float c1 = 1.70158;
    
    float c1 = 1.20158;
    
    const float c3 = c1 + 1.0;
    
    float bounceProgress = 1.0 + c3 * pow(t - 1.0, 3.0) + c1 * pow(t - 1.0, 2.0);
    
    // 시작 반지름에서 튕기는 진행도를 적용
    float startRadius = 10.0f;
    float currentRadius = lerp(startRadius, 1.0f, bounceProgress);

    float softness = 0.2f;
    float mask = smoothstep(currentRadius, currentRadius - softness, len);
    
    float darkness = 0.2f;
    float finalAlpha = lerp(darkness, 1.0f, mask);
    
    return color * finalAlpha;

}