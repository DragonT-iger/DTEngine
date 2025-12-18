// 기본적인 Lambert Lighting 모델이고 간이 환경광이 적용되어 있음.
// 텍스쳐가 있으면 텍스쳐 색상, 없으면 머티리얼 색상을 사용함.
// Forward 방식임.



struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;
};

#include "Lighting.hlsli"

cbuffer CBuffer_Material : register(b3)
{
    float4 MaterialColor; // Material.Color
    float4 UVTransform; // xy=Tiling, zw=Offset
    int UseTexture; // Material.UseTexture
    int3 Padding2;
};


Texture2D Diffuse : register(t0);
Texture2D RenderTarget : register(t1);
SamplerState g_Sampler : register(s0);

float4 PS(PS_INPUT input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    //float3 viewDir = normalize(float3(0, 0, -10) - input.WorldPos);
    
    
    clip(input.UV.x > 0.01 && input.UV.x < 0.99 ? 1 : -1);
    clip(input.UV.y > 0.04 && input.UV.y < 0.88 ? 1 : -1);
       
    
    float4 lambert = float4(ComputeLambertLighting(input.WorldPos, normal), 1);
    
    float4 finalColor;
    
    if (UseTexture)
    {
        float2 transformedUV = input.UV * UVTransform.xy + UVTransform.zw;
        
        float4 diffuse = Diffuse.Sample(g_Sampler, input.UV);
        float4 rendertarget = RenderTarget.Sample(g_Sampler, input.UV);
    
        rendertarget.rgb += MaterialColor;
        
        float4 texColor = (1 - diffuse.a) * rendertarget + diffuse.a * diffuse;
        
        
        
        clip(texColor.a - 0.01);
    
        finalColor = texColor * (lambert + 0.3);
    }
    else
    {
        finalColor = MaterialColor * (lambert + 0.3);
    }
    
    return finalColor;
}