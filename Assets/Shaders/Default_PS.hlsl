struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float3 WorldPos : TEXCOORD1;
};

struct LightData
{
    float4 PositionRange;   // xyz=Pos, w=Range
    float4 DirectionType;   // xyz=Dir, w=Type (0:Dir, 1:Point)
    float4 ColorIntensity;  // rgb=Color, w=Intensity
};

#define MAX_LIGHTS 4

cbuffer CBuffer_GlobalLight : register(b2)
{
    LightData Lights[MAX_LIGHTS];
    int ActiveCount;
    float3 Padding;
};

float4 PS(PS_INPUT input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float3 viewDir = normalize(float3(0, 0, -10) - input.WorldPos); // 임시 뷰벡터 (카메라 위치 CBuffer 필요하지만 생략)

    float3 totalDiffuse = float3(0, 0, 0);
    float3 ambient = float3(0.1, 0.1, 0.1); // 기본 환경광

    // [Loop] 활성화된 조명 개수만큼 반복
    for (int i = 0; i < ActiveCount; ++i)
    {
        float3 lightColor = Lights[i].ColorIntensity.rgb;
        float intensity = Lights[i].ColorIntensity.w;
        float type = Lights[i].DirectionType.w;

        float3 L; // 빛 방향 벡터 (To Light)
        float attenuation; // 거리 감쇠

        // 0: Directional Light
        if (type < 0.5f)
        {
            L = normalize(Lights[i].DirectionType.xyz);
            attenuation = 1.0f;
        }
        // 1: Point Light
        else
        {
            float3 toLight = Lights[i].PositionRange.xyz - input.WorldPos;
            float dist = length(toLight);
            L = normalize(toLight);

            // 거리 감쇠 (유니티 스타일: 1 / (1 + dist^2) )
            // 범위(Range)를 넘어가면 0이 되도록 처리하는 것이 좋음
            float range = Lights[i].PositionRange.w;
            float distSqr = dist * dist;
            
            attenuation = 1.0 / (1.0 + 0.1 * dist + 0.01 * distSqr);
            
            //if (dist > range)
            //    attenuation = 0.0f;
        }

        // Diffuse (Lambert)
        float NdotL = max(dot(normal, L), 0.0f);
        totalDiffuse += NdotL * lightColor * intensity * attenuation;
    }

    float3 finalColor = input.Color.rgb * (ambient + totalDiffuse);
    return float4(finalColor, 1.0f);
}