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



#include "Resource.hlsli"
#include "Lighting.hlsli"

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
    
    float3 viewDir;
    if (IsOrtho > 0.5)
    {
        viewDir = -normalize(CameraDir);
    }
    else
    {
        viewDir = normalize(CameraPos - input.WorldPos);
    }

    float4 diffuseTex = g_DiffuseMap.Sample(g_Sampler, input.UV);
    
    
    float specMask = 1.0f; //Specmap은 얘는 없음. 일단 다 반사. 

    
    //float3 ambientLight = g_CubeMap.SampleLevel(g_Sampler, normal, 0).rgb;

    
    float3 ambientLight = 0.1f;
    
    float3 totalDiffuse = float3(0, 0, 0);
    float3 totalSpecular = float3(0, 0, 0);
    float specularPower = 20.0f; // 이 값이 낮을수록 광택이 넓어짐

    float shadowFactor = CalculateShadow(input.WorldPos , Shadow_Bias);
    
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


    // 3. 최종 색상 합성
    // Specular Map이 없으므로 물체의 기본 반사(Ambient)와 광원 반사(TotalSpecular)를 합산
    float3 finalColor = (ambientLight + totalDiffuse) * diffuseTex.rgb + totalSpecular;

    return float4(finalColor, diffuseTex.a);
}