struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

cbuffer CBuffer_Material : register(b3)
{
    float4 MaterialColor;   // Material.Color
    float4 UVTransform;     // xy=Tiling, zw=Offset
    int UseTexture;         // Material.UseTexture
    int3 Padding2;
};

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

float4 PS(PS_INPUT input) : SV_Target
{
    float4 color = MaterialColor;
    
    //clip(color.a < 0.01f ? -1 : 1);
    //color.rgb = pow(color.rgb, 2.2f); SRGB 사용
    
    float4 textureColor;
    
    if (UseTexture) // 나는 베이징의 if문을 사랑해
    {
        textureColor = g_Texture.SampleLevel(g_Sampler, input.UV, 0); 
        // 밈맵 단계를 0으로 해서 텍스쳐 크기가 작아지더라도 상관없게 이것도 이상함 밈맵을 로드할때 키지도 않았는데 왜 밈멥을 원본으로 해야 안깨짐?
    }
    
    clip(textureColor.r == 1.0f && textureColor.g == 0.0f ? -1 : 1);
    // YENA와 호환성을 위해서 Clip 실제 프로젝트 할때는 알파값을 넣는게 맞다고 생각함
    // 좀 짜친다..
    
    //clip(textureColor.g < 0.009f ? -1 : 1);
    
    
    color *= textureColor;
    
    //color.rgb = pow(color.rgb, 1.0f / 2.2f);
    return color;
}