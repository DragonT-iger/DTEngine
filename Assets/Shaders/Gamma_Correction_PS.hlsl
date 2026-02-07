Texture2D mainTex : register(t0);
SamplerState samLinear : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target
{
    
   float4 BfGamma = mainTex.Sample(samLinear, input.UV);
    
    float3 color = pow(BfGamma.rgb, 1.0f / 2.2f);

    return float4(color, 1);
    
   
}