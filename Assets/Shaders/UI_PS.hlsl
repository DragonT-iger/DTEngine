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
    
    
    float4 textureColor;
    
    if (UseTexture) // 나는 베이징의 if문을 사랑해
    {
        textureColor = g_Texture.Sample(g_Sampler, input.UV);
    }
    
    clip(textureColor.g == 0.0f ? -1 : 1);
    // YENA와 호환성을 위해서
    //clip(textureColor.g < 0.009f ? -1 : 1);
    
    color *= textureColor;
    
    return color;
}