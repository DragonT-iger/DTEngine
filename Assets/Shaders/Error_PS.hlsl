struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

float4 PS(PS_INPUT input) : SV_Target
{
    return float4(1.0f, 0.0f, 1.0f, 1.0f);
}