struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float3 ViewNormal : TEXCOORD1;
};

cbuffer CBuffer_Frame : register(b0)
{
    matrix ViewTM;
    matrix ProjectionTM;
};

#include "Lighting.hlsli"

Texture2D g_DiffuseMap : register(t0);  
Texture2D g_SpecMap : register(t1);     
Texture2D g_SphereMap : register(t2);
Texture2D g_NormalMap : register(t3);

SamplerState g_Sampler : register(s0);

float4 PS(PS_INPUT input) : SV_Target
{
    float3 worldNormal = normalize(input.Normal);
    float3 worldTangent = normalize(input.Tangent);
    float3 worldBitangent = normalize(input.Bitangent);
    //float3 worldBitangent = cross(input.Normal, input.Tangent);
    
    float3 normalSample = g_NormalMap.Sample(g_Sampler, input.UV).rgb;
    float3 localNormal = normalSample * 2.0f - 1.0f;
    
    float3x3 TBN = float3x3(worldTangent, worldBitangent, worldNormal);
    float3 normal = normalize(mul(localNormal, TBN));
    
    float3 viewDir = normalize(CameraPos - input.WorldPos);

    float4 diffuseTex = g_DiffuseMap.Sample(g_Sampler, input.UV);
    
    float specMask = g_SpecMap.Sample(g_Sampler, input.UV).r;

    float3 viewNormal = normalize(mul(normal, (float3x3)ViewTM));
    float2 sphereUV = viewNormal.xy * 0.5 + 0.5;
    sphereUV.y = 1.0 - sphereUV.y;
    
    float3 ambientLight = g_SphereMap.Sample(g_Sampler, sphereUV).rgb;

    float3 totalDiffuse = float3(0, 0, 0);
    float3 totalSpecular = float3(0, 0, 0);
    float specularPower = 20.0f;

    
    float shadowFactor = CalculateShadow(input.WorldPos);
    
    for (int i = 0; i < ActiveCount; ++i)
    {
        float3 lightColor = Lights[i].ColorIntensity.rgb;
        float intensity = Lights[i].ColorIntensity.w;
        float type = Lights[i].DirectionType.w;

        float3 L;
        float attenuation = 1.0f;

        if (type < 0.5f)
        {
            L = normalize(Lights[i].DirectionType.xyz);
        }
        else
        {
            float3 toLight = Lights[i].PositionRange.xyz - input.WorldPos;
            float dist = length(toLight);
            L = normalize(toLight);
            float range = Lights[i].PositionRange.w;
            attenuation = 1.0 / (1.0 + 0.1 * dist / range + 0.01 * (dist * dist) / (range * range));
        }

        
        float NdotL = max(dot(normal, L), 0.0f);
        totalDiffuse += NdotL * lightColor * intensity * attenuation * saturate(shadowFactor + 0.5);

        if (NdotL > 0.0f)
        {
            float3 H = normalize(L + viewDir); 
            float NdotH = max(dot(normal, H), 0.0f);
            float specularTerm = pow(NdotH, specularPower);
            
            totalSpecular += specularTerm * lightColor * intensity * attenuation * specMask * shadowFactor;
        }
    }

    
    float3 finalColor = ((ambientLight * specMask) + (totalDiffuse)) * diffuseTex.rgb + totalSpecular;

    return float4(finalColor, diffuseTex.a);
}